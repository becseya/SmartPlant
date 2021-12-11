/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "app/lora/lora.hpp"
#include "mbed.h"

int main(void)
{
    Lora::onReceive([](uint8_t port, const uint8_t* buffer, size_t size) -> void {
        printf(" RX Data on port %u (%d bytes): ", port, size);

        for (uint8_t i = 0; i < size; i++) {
            printf("%02x ", buffer[i]);
        }

        printf("\r\n");
    });

    Lora::setMessageBuilder([](uint8_t* buffer, size_t size) -> int16_t {
        // simulate sensor
        static int32_t sensor_value = 0;

        printf("\r\n Dummy Sensor Value = %d \r\n", sensor_value);
        int packet_len = sprintf((char*)buffer, "%d", sensor_value);
        sensor_value++;

        return packet_len;
    });

    Lora::init();
    Lora::infiniteLoop();

    return 0;
}
