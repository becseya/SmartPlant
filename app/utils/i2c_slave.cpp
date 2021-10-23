#include "i2c_slave.hpp"

I2cSlave::I2cSlave(I2C& bus, char address_7_bit)
    : myBus(bus)
    , myAddress(address_7_bit << 1)
{}

void I2cSlave::readRegs(char reg_address, uint8_t* data, int len)
{
    myBus.write(myAddress, &reg_address, 1, true);
    myBus.read(myAddress, (char*)data, len);
}

char I2cSlave::readReg(char reg_address)
{
    uint8_t c = 0;
    readRegs(reg_address, &c, 1);
    return c;
}

void I2cSlave::writeReg(char reg_address, uint8_t data)
{
    char buffer[2];

    buffer[0] = reg_address;
    buffer[1] = data;

    myBus.write(myAddress, (char*)buffer, sizeof(buffer));
}
