#pragma once

#include "utils/log.hpp"

namespace SmartPlant {

class Sensor
{
  public:
    Sensor(const char* name);

    const char* getName();

    virtual bool init()   = 0;
    virtual void update() = 0;

  protected:
    const char* const myName;
};

#define LOG_SENSOR(format, ...) LOG("%s: " format, myName, __VA_ARGS__)

} // namespace SmartPlant
