#include "MMA8451Q.hpp"

#include "../utils/log.hpp"

#define SENSOR_NAME "ACCELEROMETER"

#define DEVICE_ADDRESS 0x1C
#define VAL_WHO_AM_I   0x1A
#define REG_WHO_AM_I   0x0D
#define REG_CTRL_REG_1 0x2A
#define REG_OUT_X_MSB  0x01
#define REG_OUT_Y_MSB  0x03
#define REG_OUT_Z_MSB  0x05

#define UINT14_MAX ((1 << 14) - 1)
#define G_SCALE    (4096.0)

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

MMA8451Q::MMA8451Q(I2C& bus)
    : I2cSlave(bus, DEVICE_ADDRESS)
    , Sensor(SENSOR_NAME)
    , aggregatorX(SENSOR_NAME "_X", false)
    , aggregatorY(SENSOR_NAME "_Y", false)
    , aggregatorZ(SENSOR_NAME "_Z", false)
{}

bool MMA8451Q::init()
{
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

    LOG_SENSOR("X=%f Y=%f Z=%f", data.x, data.y, data.z);
}

void MMA8451Q::readAxesData(axes_data_t* data)
{
    uint8_t register_buffer[6];

    readRegs(REG_OUT_X_MSB, register_buffer, sizeof(register_buffer));
    data->x = parse_register_to_float(register_buffer);
    data->y = parse_register_to_float(register_buffer + 2);
    data->z = parse_register_to_float(register_buffer + 4);
}
