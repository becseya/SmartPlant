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
    ModeSelector(PinName pin);

    void update();
    Mode getMode(bool refresh = true);
    void sleep();

  private:
    InterruptIn myButton;
    bool        myButtonPressed;
    unsigned    myIdx;
};

} // namespace SmartPlant
