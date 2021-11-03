#pragma once

#include "../sensor.hpp"
#include "mbed.h"

namespace SmartPlant {
namespace Sensors {

class Brightness : public Sensor
{
  public:
    Brightness(PinName pin);

    float readPercentage();
    bool  init() final;
    void  update() final;

  private:
    float calculatePercentage(unsigned short raw);

    AnalogIn analog;
};

} // namespace Sensors
} // namespace SmartPlant
