#include "SoilMoisture.hpp"

#include "../utils/misc.hpp"

#define SOIL_MOISTURE_VALUE_MIN (50)
#define SOIL_MOISTURE_VALUE_MAX (10800)

// --------------------------------------------------------------------------------------------------------------------

using namespace SmartPlant::Sensors;

SoilMoisture::SoilMoisture(PinName pin)
    : Sensor("SOIL MOISTURE")
    , analog(pin)
{}

float SoilMoisture::readPercentage()
{
    return calculatePercentage(analog.read_u16());
}

bool SoilMoisture::init()
{
    return true;
}

void SoilMoisture::update()
{
    unsigned short raw = analog.read_u16();

    LOG_SENSOR("%.0f %%  (%u)", calculatePercentage(raw), raw);
}

float SoilMoisture::calculatePercentage(unsigned short raw)
{
    return getCappedPercentage(raw, SOIL_MOISTURE_VALUE_MIN, SOIL_MOISTURE_VALUE_MAX);
}
