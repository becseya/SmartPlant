#pragma once

#include "../aggregation/ScalarAggregator.hpp"
#include "../sensor.hpp"
#include "mbed.h"

namespace SmartPlant {
namespace Sensors {

using Aggregation::ScalarAggregator;

class SoilMoisture : public Sensor
{
  public:
    ScalarAggregator aggregator;

    SoilMoisture(PinName pin);

    float readPercentage();
    bool  init() final;
    void  update() final;

  private:
    float calculatePercentage(unsigned short raw);

    // TODO 3: Copy aggregation the same way as it was done for Brightness
    AnalogIn analog;
};

} // namespace Sensors
} // namespace SmartPlant