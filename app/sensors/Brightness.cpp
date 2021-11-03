#include "Brightness.hpp"

#include "../utils/misc.hpp"

#define LIGHT_SCALING_VALUE_MIN (30)
#define LIGHT_SCALING_VALUE_MAX (4000)

// --------------------------------------------------------------------------------------------------------------------

using namespace SmartPlant::Sensors;

Brightness::Brightness(PinName pin)
    : Sensor("BRIGHTNESS")
    , analog(pin)
{}

float Brightness::readPercentage()
{
    return calculatePercentage(analog.read_u16());
}

bool Brightness::init()
{
    return true;
}

void Brightness::update()
{
    unsigned short raw = analog.read_u16();

    LOG_SENSOR("%.0f %%   (%u)", calculatePercentage(raw), raw);
}

float Brightness::calculatePercentage(unsigned short raw)
{
    return getCappedPercentage(raw, LIGHT_SCALING_VALUE_MIN, LIGHT_SCALING_VALUE_MAX);
}
