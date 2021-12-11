/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "app/lora/lora.hpp"
#include "app/utils/log.hpp"
#include "mbed.h"

int main(void)
{
    Lora::onReceive([](uint8_t port, const uint8_t* buffer, size_t size) -> void {
        LOG_DEBUG_BUFFER(buffer, size, " RX Data on port %u: ", port);
    });

    Lora::setMessageBuilder([](uint8_t* buffer, size_t size) -> int16_t {
        // simulate sensor
        static int32_t sensor_value = 0;

        LOG_DEBUG("Dummy Sensor Value = %d", sensor_value);
        int packet_len = sprintf((char*)buffer, "%d", sensor_value);
        sensor_value++;

        LOG_DEBUG_BUFFER(buffer, packet_len, "TX Data: ");
        return packet_len;
    });

    Lora::init();
    Lora::infiniteLoop();

    return 0;
}
