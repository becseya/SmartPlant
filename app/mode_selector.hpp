#pragma once

#include "SleepInterrupter.hpp"
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
    Mode getMode();
    void sleep(SleepInterrupter& interrupter);

  private:
    void showModeOnLeds();

    InterruptIn myButton;
    BusOut&     myLeds;
    bool        myButtonPressed;
    unsigned    myIdx;
};

} // namespace SmartPlant
