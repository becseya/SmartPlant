#pragma once

#include "../sensor.hpp"
#include "mbed.h"

namespace SmartPlant {
namespace Sensors {

class Si7021 : public Sensor
{
    public:
    Si7021(PinName sda, PinName scl);

    int32_t get_temperature();
    uint32_t get_humidity();
    bool measure();
    bool check();
    bool init() final;
    void update() final;

    private:
    I2C i2c;
    
    uint8_t  rx_buff[8];
    uint8_t  tx_buff[2];
 
    uint32_t rhData;
    int32_t  tData;

};

} // namespace Sensors
} // namespace SmartPlant
 
