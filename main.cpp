/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "app/mode_selector.hpp"
#include "mbed.h"

using namespace SmartPlant;

// Hardware elements and other classes
BusOut       modeLeds(LED1, LED2);
ModeSelector modeSelector(PB_2, modeLeds);

// -------------------------------------------------- START OF MAIN ---------------------------------------------------

int main()
{
    while (true) {
        // update
        modeSelector.update();

        // sleep
        modeSelector.sleep();
    }
}
