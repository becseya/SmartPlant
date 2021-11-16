#pragma once

#include "../sensor.hpp"
#include "../utils/LineBufferedSerial.hpp"

namespace SmartPlant {
namespace Sensors {

struct GpsInfo
{
    float lat, lon;
    float speed, heading;
    bool  valid;
    tm    time;
};

class Gps : public LineBufferedSerial<128>, public Sensor
{
  public:
    Gps(PinName tx, PinName rx);

    bool init() final;
    void update() final;
    const GpsInfo& getLastMeasurement();

  private:
    static bool        parseLine(const char* line, GpsInfo* info);
    static bool        checkLine(const char* line);
    static uint8_t     calculateChx(const char* line);
    static const char* nextField(const char* ptr);

    GpsInfo lastInfo;
};

} // namespace Sensors
} // namespace SmartPlant
