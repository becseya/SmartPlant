#include "mode_selector.hpp"

#include "utils/array.hpp"
#include "utils/log.hpp"

using namespace SmartPlant;

// --------------------------------------------------------------------------------------------------------------------

#define UPDATE_PERIOD      1s
#define SECONDS_PER_UPDATE 1

#define SLEEP_TEST_SEC   2
#define SLEEP_NORMAL_SEC 30

const auto AVALIABLE_MODES = make_array<Mode>(Mode::Test, Mode::Normal);

// --------------------------------------------------------------------------------------------------------------------

const char* SmartPlant::modeToStr(Mode mode)
{
    switch (mode) {
        case Mode::Test: return "Test";
        case Mode::Normal: return "Normal";
    }
}

static unsigned getSleepSeconds(Mode mode)
{
    switch (mode) {
        case Mode::Test: return SLEEP_TEST_SEC;
        case Mode::Normal: return SLEEP_NORMAL_SEC;
    }
}

// --------------------------------------------------------------------------------------------------------------------

ModeSelector::ModeSelector(EventQueue& globalEvents, PinName pin, BusOut& leds)
    : myButton(pin)
    , myLeds(leds)
    , myButtonPressed(false)
    , myIdx(0)
    , secondsPassed(0)
    , tickCallback(nullptr)
{
    myButton.fall([&]() -> void { //
        if (!myButtonPressed) {
            globalEvents.call([&]() -> void { onButtonPress(); });
            myButtonPressed = true;
        }
    });

    globalEvents.call_every(UPDATE_PERIOD, [&]() -> void { update(); });

    showModeOnLeds();
}

void ModeSelector::onButtonPress()
{
    if (++myIdx == AVALIABLE_MODES.size())
        myIdx = 0;

    LOG_DEBUG("New mode is: %s", modeToStr(getMode()))
    update(true);
    showModeOnLeds();
    myButtonPressed = false;
}

void ModeSelector::update(bool force)
{
    secondsPassed += SECONDS_PER_UPDATE;

    if (force || (secondsPassed >= getSleepSeconds(getMode()))) {
        if (tickCallback != nullptr)
            tickCallback(getMode());

        secondsPassed = 0;
    }
}

void ModeSelector::onTick(tick_function_t tickCallback)
{
    this->tickCallback = tickCallback;
}

Mode ModeSelector::getMode()
{
    return AVALIABLE_MODES[myIdx];
}

void ModeSelector::showModeOnLeds()
{
    myLeds = (1 << myIdx);
}
