#include "mode_selector.hpp"

#include "utils/array.hpp"
#include "utils/log.hpp"

using namespace SmartPlant;

// --------------------------------------------------------------------------------------------------------------------

#define SLEEP_TEST   2s
#define SLEEP_NORMAL 30s

const auto AVALIABLE_MODES = make_array<Mode>(Mode::Test, Mode::Normal);

// --------------------------------------------------------------------------------------------------------------------

const char* SmartPlant::modeToStr(Mode mode)
{
    switch (mode) {
        case Mode::Test: return "Test";
        case Mode::Normal: return "Normal";
    }
}

ModeSelector::ModeSelector(PinName pin)
    : myButton(pin)
    , myButtonPressed(false)
    , myIdx(0)
{
    myButton.fall([&]() -> void { myButtonPressed = true; });
}

void ModeSelector::update()
{
    if (myButtonPressed) {
        if (++myIdx == AVALIABLE_MODES.size())
            myIdx = 0;

        LOG_DEBUG("New mode is: %s", modeToStr(getMode(false)))

        myButtonPressed = false;
    }
}

Mode ModeSelector::getMode(bool refresh)
{
    if (refresh)
        update();

    return AVALIABLE_MODES[myIdx];
}

void ModeSelector::sleep()
{
    switch (getMode()) {
        case Mode::Test: ThisThread::sleep_for(SLEEP_TEST); break;
        case Mode::Normal: ThisThread::sleep_for(SLEEP_NORMAL); break;
    }
}
