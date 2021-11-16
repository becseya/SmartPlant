#include "i2c_slave.hpp"
#include <cstdint>

I2cSlave::I2cSlave(I2C& bus, char address_7_bit)
    : myBus(bus)
    , myAddress(address_7_bit << 1)
{}

int I2cSlave::read(uint8_t* data, int len)
{
    return myBus.read(myAddress, (char*)data, len);
}

int I2cSlave::write(const uint8_t* data, int len, bool repeated)
{
    return myBus.write(myAddress, (const char*)data, len, repeated);
}

void I2cSlave::readRegs(char reg_address, uint8_t* data, int len)
{
    write((uint8_t*)&reg_address, 1, true);
    read(data, len);
}

char I2cSlave::readReg16(uint16_t reg_address)
{
    uint8_t buffer[2];

    buffer[0] = reg_address >> 8;
    buffer[1] = reg_address;
    write(buffer, sizeof(buffer), true);
    read(buffer, 1);

    return buffer[0];
}

char I2cSlave::readReg(char reg_address)
{
    uint8_t c = 0;
    readRegs(reg_address, &c, 1);
    return c;
}

void I2cSlave::writeReg(char reg_address, uint8_t data)
{
    uint8_t buffer[2];

    buffer[0] = reg_address;
    buffer[1] = data;

    write(buffer, sizeof(buffer));
}

void I2cSlave::writeRegs(char reg_address, const uint8_t* data, int len)
{
    uint8_t tx_buffer[len + 1];

    tx_buffer[0] = reg_address;

    for (int i = 0; i < len; i++)
        tx_buffer[i + 1] = data[i];

    write(tx_buffer, len + 1);
}
