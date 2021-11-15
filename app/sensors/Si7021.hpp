#pragma once

#include "../sensor.hpp"
#include "mbed.h"

namespace SmartPlant {
namespace Sensors {

struct TempHumData
{
    float temp;
    float humidity;
};

class Si7021 : public Sensor
{
  public:
    Si7021(PinName sda, PinName scl);

    bool measure(TempHumData* data);
    bool check();
    bool init() final;
    void update() final;

  private:
    I2C i2c;

    uint8_t rx_buff[8];
    uint8_t tx_buff[2];
};

} // namespace Sensors
} // namespace SmartPlant
