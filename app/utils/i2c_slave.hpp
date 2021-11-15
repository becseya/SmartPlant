#pragma once

#include "mbed.h"
#include <cstdint>

class I2cSlave
{
  public:
    I2cSlave(I2C& bus, char address_7_bit);

  protected:
    void readRegs(char reg_address, uint8_t* data, int len);
    char readReg16(uint16_t reg_address);
    char readReg(char reg_address);
    void writeReg(char reg_address, uint8_t data);

  private:
    I2C& myBus;
    char myAddress;
};
