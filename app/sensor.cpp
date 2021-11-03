#include "sensor.hpp"

using namespace SmartPlant;

Sensor::Sensor(const char* name)
    : myName(name)
{}

const char* Sensor::getName()
{
    return myName;
}
