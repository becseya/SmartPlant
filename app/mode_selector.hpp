#pragma once

#include "EventQueue.hpp"
#include "mbed.h"
#include <functional>

namespace SmartPlant {

enum class Mode
{
    Test,
    Normal
};

extern const char* modeToStr(Mode mode);

class ModeSelector
{
    using tick_function_t = std::function<void(Mode currentMode)>;

  public:
    ModeSelector(EventQueue& globalEvents, PinName pin, BusOut& leds);

    Mode getMode();
    void onTick(tick_function_t callback);

  private:
    void onButtonPress();
    void update(bool force = false);
    void showModeOnLeds();

    InterruptIn     myButton;
    BusOut&         myLeds;
    bool            myButtonPressed;
    unsigned        myIdx;
    unsigned        secondsPassed;
    tick_function_t tickCallback;
};

} // namespace SmartPlant
