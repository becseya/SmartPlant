#pragma once

#include "../sensor.hpp"
#include "../utils/i2c_slave.hpp"
#include "mbed.h"

namespace SmartPlant {
namespace Sensors {

struct TempHumData
{
    float temp;
    float humidity;
};

class Si7021 : protected I2cSlave, public Sensor
{
  public:
    Si7021(I2C& bus);

    void measure(TempHumData* data);
    bool init() final;
    void update() final;
};

} // namespace Sensors
} // namespace SmartPlant
