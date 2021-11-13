#pragma once

#include "../sensor.hpp"
#include "mbed.h"

namespace SmartPlant {
namespace Sensors {

class SoilMoisture : public Sensor
{
  public:
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