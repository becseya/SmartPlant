#include "RGBLed.hpp"

using namespace SmartPlant;

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

void RGBLed::turnOff()
{
    pinR = 0;
    pinG = 0;
    pinB = 0;
}
