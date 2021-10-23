#include "mode_selector.hpp"

#include "utils/array.hpp"
#include "utils/log.hpp"

using namespace SmartPlant;

// --------------------------------------------------------------------------------------------------------------------

#define SLEEP_QUANTA         100ms
#define SLEEP_QUANTA_PER_SEC 10

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
    unsigned i      = 0;
    unsigned quanta = SLEEP_QUANTA_PER_SEC * getSleepSeconds(getMode());

    while ((i++ < quanta) && !myButtonPressed) {
        ThisThread::sleep_for(SLEEP_QUANTA);
    }
}
