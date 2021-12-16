#include "RGBLed.hpp"
#include <cstring>

using namespace SmartPlant;

static bool isBufferEqual(const char* str, const char* buffer, size_t size)
{
    if (strlen(str) != size)
        return false;

    return strstr(buffer, str);
}

RGBLed::RGBLed(PinName PinR, PinName PinG, PinName PinB)
    : pinR(PinR)
    , pinG(PinG)
    , pinB(PinB)
{}

void RGBLed::setColor(Color color)
{
    switch (color) {
        case COLOR_RED:
            pinR = 1;
            pinG = 0;
            pinB = 0;
            break;
        case COLOR_GREEN:
            pinR = 0;
            pinG = 1;
            pinB = 0;
            break;
        case COLOR_BLUE:
            pinR = 0;
            pinG = 0;
            pinB = 1;
            break;
        case COLOR_CLEAR:
            pinR = 1;
            pinG = 1;
            pinB = 1;
            break;
        default: //
            break;
    }
}

void RGBLed::parseCommand(const char* buffer, size_t size)
{
    if (isBufferEqual("Green", buffer, size))
        setColor(Color::COLOR_GREEN);
    else if (isBufferEqual("Red", buffer, size))
        setColor(Color::COLOR_RED);
    else if (isBufferEqual("OFF", buffer, size)) {
        pinR = 0;
        pinG = 0;
        pinB = 0;
    }
}

void RGBLed::setColor3Bit(uint8_t bitmask)
{
    pinR = bitmask & (1 << 0);
    pinG = bitmask & (1 << 1);
    pinB = bitmask & (1 << 2);
}
