/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "app/utils/log.hpp"
#include "mbed.h"

// Blinking rate in milliseconds
#define BLINKING_RATE 500ms

int main()
{
    DigitalOut led(LED1);

    while (true) {
        led = !led;
        LOG_DEBUG("LED is: %s", (led ? "On" : "Off"))
        ThisThread::sleep_for(BLINKING_RATE);
    }
}
