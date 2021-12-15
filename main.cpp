/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "app/lora/lora.hpp"
#include "app/utils/log.hpp"
#include "mbed.h"

// minimal for LoRa: 10
events::EventQueue globalEvents(10 * EVENTS_EVENT_SIZE);

uint8_t binary_data[] = { 0xff, 0xf0, 0x00, 0x2a, 0x03, 0xe6, 0x00, 0x01, 0x00, 0x01, 0x02, 0x61, 0x00, 0xb0,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x76, 0x14, 0xfc, 0x43 };

int main(void)
{
    Lora::onReceive([](uint8_t port, const uint8_t* buffer, size_t size) -> void {
        LOG_DEBUG_BUFFER(buffer, size, " RX Data on port %u: ", port);
    });

    Lora::setMessageBuilder([](uint8_t* buffer, size_t size) -> int16_t {
        // simulate sensor
        static uint8_t sensor_value = 0;

        binary_data[7] = sensor_value;
        binary_data[9] = sensor_value;
        sensor_value++;

        size_t packet_len = sizeof(binary_data);
        memcpy(buffer, binary_data, packet_len);
        LOG_DEBUG_BUFFER(buffer, packet_len, "TX Data: ");
        return packet_len;
    });

    Lora::init(globalEvents);

    globalEvents.dispatch_forever();

    return 0;
}
