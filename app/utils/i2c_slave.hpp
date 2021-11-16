#pragma once

#include "mbed.h"
#include <cstdint>

class I2cSlave
{
  public:
    I2cSlave(I2C& bus, char address_7_bit);

  protected:
    int read(uint8_t* data, int len);
    int write(const uint8_t* data, int len, bool repeated = false);

    void readRegs(char reg_address, uint8_t* data, int len);
    char readReg16(uint16_t reg_address);
    char readReg(char reg_address);
    void writeReg(char reg_address, uint8_t data);
    void writeRegs(char reg_address, const uint8_t* data, int len);

  private:
    I2C& myBus;
    char myAddress;
};
