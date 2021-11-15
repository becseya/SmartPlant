#include "Si7021.hpp"

#include "../utils/log.hpp"

#define READ_TEMP  0xE0
#define READ_RH    0xE5
#define READ_ID1_1 0xFA
#define READ_ID1_2 0x0F
#define READ_ID2_1 0xFC
#define READ_ID2_2 0xC9
#define ADDR       0x80
#define DEVICE_ID  0x15
#define FREQ       100000

// --------------------------------------------------------------------------------------------------------------------

using namespace SmartPlant::Sensors;

Si7021::Si7021(PinName sda, PinName scl)
    : i2c(sda, scl)
    , Sensor("TemperatureHumidity")
{
    i2c.frequency(FREQ);
}

int32_t Si7021::get_temperature()
{
    return tData;
}

uint32_t Si7021::get_humidity()
{
    return rhData;
}

bool Si7021::init()
{
    return true;
}

// Check if the sensor is active and responding

bool Si7021::check()
{
    tx_buff[0] = READ_ID2_1;
    tx_buff[1] = READ_ID2_2;
    if (i2c.write(ADDR, (char*)tx_buff, 2) != 0)
        return 0;
    if (i2c.read(ADDR, (char*)rx_buff, 8) != 0)
        return 0;

    if (rx_buff[0] == DEVICE_ID)
        return true;
    else
        return 0;
}

void Si7021::update()
{
    measure();
    LOG_SENSOR("", rhData, tData);
}

bool Si7021::measure()
{
    tx_buff[0] = READ_RH;
    if (i2c.write(ADDR, (char*)tx_buff, 1) != 0)
        return 0;
    if (i2c.read(ADDR, (char*)rx_buff, 2) != 0)
        return 0;

    rhData = ((uint32_t)rx_buff[0] << 8) + (rx_buff[1] & 0xFC);
    rhData = (((rhData)*15625L) >> 13) - 6000;

    tx_buff[0] = READ_TEMP;
    if (i2c.write(ADDR, (char*)tx_buff, 1) != 0)
        return 0;
    if (i2c.read(ADDR, (char*)rx_buff, 2) != 0)
        return 0;

    tData = ((uint32_t)rx_buff[0] << 8) + (rx_buff[1] & 0xFC);
    tData = (((tData)*21965L) >> 13) - 46850;

    return 1;
}
