#pragma once

#include "../EventQueue.hpp"
#include "../aggregation/ScalarAggregator.hpp"
#include "../sensor.hpp"
#include "../utils/i2c_slave.hpp"

namespace SmartPlant {
namespace Sensors {

using Aggregation::ScalarAggregator;

struct axes_data_t
{
    float x;
    float y;
    float z;
};

class MMA8451Q : protected I2cSlave, public Sensor
{
  public:
    ScalarAggregator aggregatorX;
    ScalarAggregator aggregatorY;
    ScalarAggregator aggregatorZ;

    MMA8451Q(I2C& bus, PinName interruptPin, EventQueue& globalEvents);

    void readAxesData(axes_data_t* data);
    bool init() final;
    void update() final;
    void handleInterrupt();

  private:
    InterruptIn interruptPin;
    bool        interrupted;
    char        lastPositionReg;
    unsigned    positionChanges;
    unsigned    tapCount;
    unsigned    freeFallCount;
};

} // namespace Sensors
} // namespace SmartPlant
