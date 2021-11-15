#include "Si7021.hpp"
#include <cstdint>

#define DEVICE_ADDRESS 0x40
#define DEVICE_ID      0x15

#define READ_TEMP 0xE0
#define READ_RH   0xE5
#define READ_ID2  0xFCC9

// --------------------------------------------------------------------------------------------------------------------

using namespace SmartPlant::Sensors;

Si7021::Si7021(I2C& bus)
    : I2cSlave(bus, DEVICE_ADDRESS)
    , Sensor("TEMP/HUM")
    , aggregatorTemp("TEMPERATURE", true)
    , aggregatorHumidity("HUMIDITY", true)
{}

bool Si7021::init()
{
    char id = readReg16(READ_ID2);

    LOG_DEBUG("ID: 0x%x", id);

    return (id == DEVICE_ID);
}

void Si7021::update()
{
    TempHumData data;

    measure(&data);
    aggregatorTemp.addSample(data.temp);
    aggregatorHumidity.addSample(data.humidity);

    LOG_SENSOR("%.1f C, %.1f %%", data.temp, data.humidity);
}

void Si7021::measure(TempHumData* data)
{
    int16_t reg16;
    uint8_t buffer[2];

    readRegs(READ_RH, buffer, sizeof(buffer));
    uint32_t rhData = ((uint32_t)buffer[0] << 8) + (buffer[1] & 0xFC);
    rhData          = (((rhData)*15625L) >> 13) - 6000;
    data->humidity  = (float)rhData / 1000;

    readRegs(READ_TEMP, buffer, sizeof(buffer));
    int32_t tData = ((uint32_t)buffer[0] << 8) + (buffer[1] & 0xFC);
    tData         = (((tData)*21965L) >> 13) - 46850;
    data->temp    = (float)tData / 1000;
}
