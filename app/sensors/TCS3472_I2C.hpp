#pragma once

#include "../Color.hpp"
#include "../aggregation/ColorAggregator.hpp"
#include "../sensor.hpp"
#include "../utils/i2c_slave.hpp"
#include "mbed.h"

namespace SmartPlant {
namespace Sensors {

using Aggregation::ColorAggregator;

struct ColorData
{
    int   r, g, b;
    int   clear;
    Color dominantColor;
};

class TCS3472_I2C : public Sensor, protected I2cSlave
{
  public:
    ColorAggregator aggregator;

    TCS3472_I2C(I2C& bus);

    bool init() final;
    void update() final;

    const ColorData& getLastMeasurement();
    void             measure(ColorData* data);

    int   enablePower();
    int   disablePower();
    bool  isPowerEnabled();
    int   enableRGBC();
    int   disableRGBC();
    bool  isRGBCEnabled();
    int   enablePowerAndRGBC();
    int   disablePowerAndRGBC();
    int   enableWait();
    int   disableWait();
    bool  isWaitEnabled();
    int   enableInterrupt();
    int   disableInterrupt();
    bool  isInterruptEnabled();
    int   setWaitTime(const float wtime);
    float readWaitTime();
    int   setIntegrationTime(const float itime);
    float readIntegrationTime();
    char  readEnableRegister();
    int   readLowInterruptThreshold();
    int   readHighInterruptThreshold();
    int   setLowInterruptThreshold(const int threshold);
    int   setHighInterruptThreshold(const int threshold);
    int   readInterruptPersistence();
    int   setInterruptPersistence(const int persistence);
    int   clearInterrupt();
    int   readRGBCGain();
    int   setRGBCGain(const int gain);
    char  getDeviceID();
    char  readStatusRegister();

  private:
    int  writeSingleRegister(char address, char data);
    int  writeMultipleRegisters(char address, char* data, int quantity);
    char readSingleRegister(char address);
    int  readMultipleRegisters(char address, char* output, int quantity);

    Color calculateDominantColor(int r, int g, int b);

    float roundTowardsZero(const float value);

    ColorData lastMeasurement;
};

} // namespace Sensors
} // namespace SmartPlant
