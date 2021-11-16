#include "mode_selector.hpp"

#include "utils/array.hpp"
#include "utils/log.hpp"

using namespace SmartPlant;

// --------------------------------------------------------------------------------------------------------------------

#define SLEEP_QUANTA         10ms
#define SLEEP_QUANTA_PER_SEC 100

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

ModeSelector::ModeSelector(PinName pin, BusOut& leds)
    : myButton(pin)
    , myLeds(leds)
    , myButtonPressed(false)
    , myIdx(0)
{
    myButton.fall([&]() -> void { myButtonPressed = true; });
    showModeOnLeds();
}

void ModeSelector::update()
{
    if (myButtonPressed) {
        if (++myIdx == AVALIABLE_MODES.size())
            myIdx = 0;

        showModeOnLeds();
        LOG_DEBUG("New mode is: %s", modeToStr(getMode()))

        myButtonPressed = false;
    }
}

Mode ModeSelector::getMode()
{
    return AVALIABLE_MODES[myIdx];
}

void ModeSelector::showModeOnLeds()
{
    myLeds = (1 << myIdx);
}

void ModeSelector::sleep(SleepInterrupter& interrupter)
{
    update();

    unsigned i      = 0;
    unsigned quanta = SLEEP_QUANTA_PER_SEC * getSleepSeconds(getMode());

    while ((i++ < quanta) && !myButtonPressed) {
        if (interrupter.isInterrupted())
            interrupter.handleInterrupt();

        ThisThread::sleep_for(SLEEP_QUANTA);
    }
}
