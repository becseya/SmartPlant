#pragma once

#include "Color.hpp"
#include "mbed.h"

namespace SmartPlant {

class RGBLed
{
  public:
    RGBLed(PinName PinR, PinName PinG, PinName PinB);

    void setColor(Color color);
    void setColor3Bit(uint8_t bitmask);

  private:
    DigitalOut pinR;
    DigitalOut pinG;
    DigitalOut pinB;
};

} // namespace SmartPlant
