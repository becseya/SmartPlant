#include "MMA8451Q.hpp"

#include "../utils/log.hpp"

#define SENSOR_NAME "ACCELEROMETER"

#define DEVICE_ADDRESS 0x1C
#define VAL_WHO_AM_I   0x1A
#define REG_WHO_AM_I   0x0D
#define REG_CTRL_REG_1 0x2A
#define REG_CTRL_REG_4 0x2D
#define REG_OUT_X_MSB  0x01
#define REG_OUT_Y_MSB  0x03
#define REG_OUT_Z_MSB  0x05

#define REG_PL_STATUS   0x10
#define REG_PL_CFG      0x11
#define REG_PL_DEBOUNCE 0x12
#define FLAG_PL_NEW     (1 << 7)
#define MASK_PL_LOCKOUT 0b01000000
#define MASK_PL_LAPO    0b00000110

#define REG_PULSE_CFG       0x21
#define REG_PULSE_SRC       0x22
#define FLAG_PULSE_EA       (1 << 7)
#define FLAG_PULSE_Z_SINGLE (1 << 4)

#define REG_FF_MT_CFG 0x15
#define REG_FF_MT_SRC 0x16
#define REG_FF_MT_THS 0x17
#define FLAG_FF_EA    (1 << 7)

#define REG_INT_SOURCE     0x0C
#define FLAG_INT_EN_LNDPRT (1 << 4)
#define FLAG_INT_EN_PULSE  (1 << 3)
#define FLAG_INT_EN_FF_MT  (1 << 2)

#define UINT14_MAX ((1 << 14) - 1)
#define G_SCALE    (4096.0)

#define G_LIMIT_A_LOW  (0.8)
#define G_LIMIT_A_HIGH (1.2)
#define G_LIMIT_B_LOW  (-0.2)
#define G_LIMIT_B_HIGH (0.2)

static float parse_register_to_float(uint8_t* ptr)
{
    int16_t int_value;

    int_value = (ptr[0] << 6) | (ptr[1] >> 2);
    if (int_value > (UINT14_MAX / 2))
        int_value -= UINT14_MAX;

    return (float)int_value / G_SCALE;
}

// --------------------------------------------------------------------------------------------------------------------

using namespace SmartPlant::Sensors;

MMA8451Q::MMA8451Q(I2C& bus, PinName interruptPin)
    : I2cSlave(bus, DEVICE_ADDRESS)
    , Sensor(SENSOR_NAME)
    , aggregatorX(SENSOR_NAME "_X", false, G_LIMIT_B_LOW, G_LIMIT_B_HIGH)
    , aggregatorY(SENSOR_NAME "_Y", false, G_LIMIT_B_LOW, G_LIMIT_B_HIGH)
    , aggregatorZ(SENSOR_NAME "_Z", false, G_LIMIT_A_LOW, G_LIMIT_A_HIGH)
    , interruptPin(interruptPin)
    , interrupted(false)
    , lastPositionReg(0)
    , positionChanges(0)
    , tapCount(0)
    , freeFallCount(0)
{
    this->interruptPin.fall([&]() -> void { interrupted = true; });
}

bool MMA8451Q::init()
{
    // dectivate device
    writeReg(REG_CTRL_REG_1, 0x00);

    // Setup extra functionalities
    writeReg(REG_PL_CFG, 0b11000000);             // enable detection, clear debounce on state change
    writeReg(REG_PL_DEBOUNCE, 255);               // set maximal debounce to reduce noise
    writeReg(REG_PULSE_CFG, FLAG_PULSE_Z_SINGLE); // enable single tap detection on Z axis
    writeReg(REG_FF_MT_CFG, 0b10111000);          // enable freefall detection on all axes
    writeReg(REG_FF_MT_THS, 0x03);                // approx 0.2 g threshold

    // enable interrupts
    writeReg(REG_CTRL_REG_4, FLAG_INT_EN_LNDPRT | FLAG_INT_EN_PULSE | FLAG_INT_EN_FF_MT);

    // activate the device
    writeReg(REG_CTRL_REG_1, 0x01);

    // check identity
    char who = readReg(REG_WHO_AM_I);
    LOG_DEBUG("I am: 0x%2X", who);
    return (who == VAL_WHO_AM_I);
}

void MMA8451Q::update()
{
    axes_data_t data;

    readAxesData(&data);
    aggregatorX.addSample(data.x);
    aggregatorY.addSample(data.y);
    aggregatorZ.addSample(data.z);

    // Resettign the board, but not the accelerometer can cause interrupt signals to stuck
    // calling this fuction regularly makes sure we clear them inside the accelometer
    handleInterrupt();

    LOG_SENSOR("X=%f Y=%f Z=%f", data.x, data.y, data.z);
    LOG_SENSOR("Position changes: %u, Taps: %u, Free falls: %u", positionChanges, tapCount, freeFallCount);
}

void MMA8451Q::readAxesData(axes_data_t* data)
{
    uint8_t register_buffer[6];

    readRegs(REG_OUT_X_MSB, register_buffer, sizeof(register_buffer));
    data->x = parse_register_to_float(register_buffer);
    data->y = parse_register_to_float(register_buffer + 2);
    data->z = parse_register_to_float(register_buffer + 4);
}

bool MMA8451Q::isInterrupted()
{
    return interrupted;
}

void MMA8451Q::handleInterrupt()
{
    char reg_interrupts = readReg(REG_INT_SOURCE);

    if (reg_interrupts & FLAG_INT_EN_LNDPRT) {
        char positionReg = readReg(REG_PL_STATUS);

        if (lastPositionReg != 0) {          // ignore the first change after startup
            if (positionReg & FLAG_PL_NEW) { // change reported by accelerometer

                // we care if lockout has changed, or the landcape/orientation statusI
                if ((positionReg & MASK_PL_LOCKOUT) != (lastPositionReg & MASK_PL_LOCKOUT) ||
                    (positionReg & MASK_PL_LAPO) != (lastPositionReg & MASK_PL_LAPO)) //
                {
                    positionChanges++;
                }
            }
        }

        lastPositionReg = positionReg;
    }
    if (reg_interrupts & FLAG_INT_EN_PULSE) {
        char pulseReg = readReg(REG_PULSE_SRC);

        if ((pulseReg & FLAG_PULSE_EA) && (pulseReg & FLAG_PULSE_EA))
            tapCount++;
    }
    if (reg_interrupts & FLAG_INT_EN_FF_MT) {
        char ffReg = readReg(REG_FF_MT_SRC);

        if (ffReg & FLAG_PULSE_EA)
            freeFallCount++;
    }

    interrupted = false;
}
