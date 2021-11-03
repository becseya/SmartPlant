#include "Gps.hpp"

using namespace SmartPlant::Sensors;

Gps::Gps(PinName tx, PinName rx)
    : LineBufferedSerial<128>(tx, rx)
    , Sensor("GPS")
{}

bool Gps::init()
{
    uart.set_baud(9600);
    uart.set_format(8, BufferedSerial::None, 1);

    return true;
}

void Gps::update()
{
    const char* line;

    while ((line = readNextLine())) {
        LOG_DEBUG("%s", line);
    }
}
