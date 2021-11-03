#pragma once

#include "../sensor.hpp"
#include "../utils/LineBufferedSerial.hpp"

namespace SmartPlant {
namespace Sensors {

class Gps : public LineBufferedSerial<128>, public Sensor
{
  public:
    Gps(PinName tx, PinName rx);

    bool init() final;
    void update() final;
};

} // namespace Sensors
} // namespace SmartPlant
