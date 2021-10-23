/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "app/mode_selector.hpp"
#include "app/sensor.hpp"
#include "app/utils/array.hpp"
#include "app/utils/log.hpp"
#include "mbed.h"

using namespace SmartPlant;

struct DummySensor : Sensor
{
    DummySensor()
        : Sensor("DUMMY")
    {}

    bool init() final { return true; }
    void update() final { LOG_SENSOR("%s", "This is a test"); }
};

// Hardware elements and other classes
BusOut       modeLeds(LED1, LED2);
ModeSelector modeSelector(PB_2, modeLeds);

// Sensors
DummySensor dummy;

const auto sensors = make_array<Sensor*>( //
    &dummy);

// -------------------------------------------------- START OF MAIN ---------------------------------------------------

int main()
{
    // init
    LOG_DEBUG("Initializing %u sensors...", sensors.size())
    for (auto& s : sensors) {
        if (!s->init())
            LOG("Failed to initialize %s!", s->getName())
    }

    while (true) {
        // update
        modeSelector.update();
        for (auto& s : sensors) {
            s->update();
        }

        // sleep
        modeSelector.sleep();
    }
}
