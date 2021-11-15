#include "Brightness.hpp"

#include "../utils/misc.hpp"

#define SENSOR_NAME             "BRIGHTNESS"
#define LIGHT_SCALING_VALUE_MIN (30)
#define LIGHT_SCALING_VALUE_MAX (4000)

// --------------------------------------------------------------------------------------------------------------------

using namespace SmartPlant::Sensors;

Brightness::Brightness(PinName pin)
    : Sensor(SENSOR_NAME)
    , analog(pin)
    , aggregator(SENSOR_NAME, true)
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
    unsigned short raw        = analog.read_u16();
    float          brightness = calculatePercentage(raw);

    aggregator.addSample(brightness);

    LOG_SENSOR("%.0f %%   (%u)", brightness, raw);
}

float Brightness::calculatePercentage(unsigned short raw)
{
    return getCappedPercentage(raw, LIGHT_SCALING_VALUE_MIN, LIGHT_SCALING_VALUE_MAX);
}
