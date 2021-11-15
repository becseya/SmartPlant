#pragma once

#include "mbed.h"

namespace SmartPlant {

enum class Mode
{
    Test,
    Normal
};

extern const char* modeToStr(Mode mode);

class ModeSelector
{
  public:
    ModeSelector(PinName pin, BusOut& leds);

    void update();
    Mode getMode(bool refresh = true);
    void sleep();

  private:
    void showModeOnLeds();

    InterruptIn myButton;
    BusOut&     myLeds;
    bool        myButtonPressed;
    unsigned    myIdx;
};

} // namespace SmartPlant
