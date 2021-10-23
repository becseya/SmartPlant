/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "app/mode_selector.hpp"
#include "app/sensors/MMA8451Q.hpp"
#include "app/utils/array.hpp"
#include "app/utils/log.hpp"
#include "mbed.h"

using namespace SmartPlant;

// Hardware elements and other classes
I2C          i2cBus(PB_9, PB_8);
BusOut       modeLeds(LED1, LED2);
ModeSelector modeSelector(PB_2, modeLeds);

// Sensors
Sensors::MMA8451Q sAccelerometer(i2cBus);

const auto sensors = make_array<Sensor*>( //
    &sAccelerometer);

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
