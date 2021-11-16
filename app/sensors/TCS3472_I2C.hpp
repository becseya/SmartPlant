#pragma once

#include "../sensor.hpp"
#include "../utils/i2c_slave.hpp"
#include "mbed.h"

namespace SmartPlant {
namespace Sensors {

class TCS3472_I2C
{
  public:
    TCS3472_I2C(PinName sda, PinName scl);
    ~TCS3472_I2C();

    void  getAllColors(int* readings);
    int   getClearData();
    int   getRedData();
    int   getGreenData();
    int   getBlueData();
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
    I2C i2c;

    int  writeSingleRegister(char address, char data);
    int  writeMultipleRegisters(char address, char* data, int quantity);
    char readSingleRegister(char address);
    int  readMultipleRegisters(char address, char* output, int quantity);

    float roundTowardsZero(const float value);
};

} // namespace Sensors
} // namespace SmartPlant
