/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include <cstdio>

// Blinking rate in milliseconds
#define BLINKING_RATE 500ms

int main()
{
    DigitalOut led(LED1);

    while (true) {
        led = !led;
        printf("LED is: %s\n", (led ? "On" : "Off"));
        ThisThread::sleep_for(BLINKING_RATE);
    }
}
