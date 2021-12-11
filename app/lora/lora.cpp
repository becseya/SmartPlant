/**
 * Copyright (c) 2017, Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "events/EventQueue.h"
#include "lorawan/LoRaWANInterface.h"
#include "lorawan/system/lorawan_data_structures.h"
#include "mbed.h"

// Application helpers
#include "lora.hpp"
#include "lora_radio_helper.h"
#include "trace_helper.h"

// Inactive code depending on this compiler flag was removed
// Make sure the configuration does not change
static_assert(MBED_CONF_LORA_DUTY_CYCLE_ON, "Configuration is not compatible with this implementation");

// --------------------------------------------------------------------------------------------------------------------

using namespace events;

/**
 * Maximum number of events for the event queue.
 * 10 is the safe number for the stack events, however, if application
 * also uses the queue for whatever purposes, this number should be increased.
 */
#define MAX_NUMBER_OF_EVENTS   10
#define EVENT_QUEUE_ALLOC_SIZE (MAX_NUMBER_OF_EVENTS * EVENTS_EVENT_SIZE)

/**
 * Maximum number of retries for CONFIRMED messages before giving up
 */
#define CONFIRMED_MSG_RETRY_COUNTER 3

/**
 * This event queue is the global event queue for both the
 * application and stack. To conserve memory, the stack is designed to run
 * in the same thread as the application and the application is responsible for
 * providing an event queue to the stack that will be used for ISR deferment as
 * well as application information event queuing.
 */
static EventQueue ev_queue(EVENT_QUEUE_ALLOC_SIZE);

/**
 * Constructing Mbed LoRaWANInterface and passing it the radio object from lora_radio_helper.
 */
static LoRaWANInterface lorawan(radio);

/**
 * Application specific callbacks
 */
static lorawan_app_callbacks_t callbacks;

// Max payload size can be LORAMAC_PHY_MAXPAYLOAD.
// This example only communicates with much shorter messages (<30 bytes).
// If longer messages are used, these buffers must be changed accordingly.
uint8_t tx_buffer[30];
uint8_t rx_buffer[30];

static Lora::message_builder_t myMessageBuilder;
static Lora::receive_handler_t myReceiveHandler = nullptr;

// clang-format off
static uint8_t DEV_EUI[] = { 0x86, 0x39, 0x32, 0x35, 0x59, 0x37, 0x91, 0x94 };
static uint8_t APP_EUI[] = { 0x70, 0xb3, 0xd5, 0x7e, 0xd0, 0x00, 0xfc, 0xda };
static uint8_t APP_KEY[] = { 0xf3, 0x1c, 0x2e, 0x8b, 0xc6, 0x71, 0x28, 0x1d, 0x51, 0x16, 0xf0, 0x8f, 0xf0, 0xb7, 0x92, 0x8f };
// clang-format on

// --------------------------------------------------------------------------------------------------------------------

/**
 * Sends a message to the Network Server
 */
static void send_message()
{
    uint16_t packet_len;
    int16_t  retcode;

    if (!myMessageBuilder)
        return;

    retcode = myMessageBuilder(tx_buffer, sizeof(tx_buffer));

    if ((retcode <= 0) || (retcode > sizeof(tx_buffer))) {
        printf("Failed to build message\r\n");
        return;
    }

    packet_len = retcode;

    retcode = lorawan.send(MBED_CONF_LORA_APP_PORT, tx_buffer, packet_len, MSG_UNCONFIRMED_FLAG);

    if (retcode < 0) {
        if (retcode == LORAWAN_STATUS_WOULD_BLOCK) {
            printf("send - WOULD BLOCK\r\n");
            ev_queue.call_in(3000, send_message); // retry in 3 seconds
        } else
            printf("\r\n send() - Error code %d \r\n", retcode);

        return;
    }

    printf("\r\n %d bytes scheduled for transmission \r\n", retcode);
    memset(tx_buffer, 0, sizeof(tx_buffer));
}

/**
 * Receive a message from the Network Server
 */
static void receive_message()
{
    uint8_t port;
    int     flags;
    int16_t retcode = lorawan.receive(rx_buffer, sizeof(rx_buffer), port, flags);

    if (retcode < 0) {
        printf("\r\n receive() - Error code %d \r\n", retcode);
        return;
    }

    if (myReceiveHandler)
        myReceiveHandler(port, rx_buffer, retcode);

    memset(rx_buffer, 0, sizeof(rx_buffer));
}

/**
 * Event handler
 */
static void lora_event_handler(lorawan_event_t event)
{
    switch (event) {
        case CONNECTED:
            printf("\r\n Connection - Successful \r\n");
            send_message();
            break;
        case DISCONNECTED:
            ev_queue.break_dispatch();
            printf("\r\n Disconnected Successfully \r\n");
            break;
        case TX_DONE:
            printf("\r\n Message Sent to Network Server \r\n");
            send_message();
            break;
        case TX_TIMEOUT:
        case TX_ERROR:
        case TX_CRYPTO_ERROR:
        case TX_SCHEDULING_ERROR:
            printf("\r\n Transmission Error - EventCode = %d \r\n", event);
            // try again
            send_message();
            break;
        case RX_DONE:
            printf("\r\n Received message from Network Server \r\n");
            receive_message();
            break;
        case RX_TIMEOUT:
        case RX_ERROR: //
            printf("\r\n Error in reception - Code = %d \r\n", event);
            break;
        case JOIN_FAILURE: //
            printf("\r\n OTAA Failed - Check Keys \r\n");
            break;
        case UPLINK_REQUIRED:
            printf("\r\n Uplink required by NS \r\n");
            send_message();
            break;
        default: //
            MBED_ASSERT("Unknown Event");
    }
}

// --------------------------------------------------------------------------------------------------------------------

int Lora::init()
{
    // setup tracing
    setup_trace();

    // stores the status of a call to LoRaWAN protocol
    lorawan_status_t retcode;

    // Initialize LoRaWAN stack
    if (lorawan.initialize(&ev_queue) != LORAWAN_STATUS_OK) {
        printf("\r\n LoRa initialization failed! \r\n");
        return -1;
    }

    printf("\r\n Mbed LoRaWANStack initialized \r\n");

    // prepare application callbacks
    callbacks.events = mbed::callback(lora_event_handler);
    lorawan.add_app_callbacks(&callbacks);

    // Set number of retries in case of CONFIRMED messages
    if (lorawan.set_confirmed_msg_retries(CONFIRMED_MSG_RETRY_COUNTER) != LORAWAN_STATUS_OK) {
        printf("\r\n set_confirmed_msg_retries failed! \r\n\r\n");
        return -1;
    }

    printf("\r\n CONFIRMED message retries : %d \r\n", CONFIRMED_MSG_RETRY_COUNTER);

    // Enable adaptive data rate
    if (lorawan.enable_adaptive_datarate() != LORAWAN_STATUS_OK) {
        printf("\r\n enable_adaptive_datarate failed! \r\n");
        return -1;
    }

    printf("\r\n Adaptive data  rate (ADR) - Enabled \r\n");
    lorawan_connect_t connect_params;
    connect_params.connect_type                = LORAWAN_CONNECTION_OTAA;
    connect_params.connection_u.otaa.dev_eui   = DEV_EUI;
    connect_params.connection_u.otaa.app_eui   = APP_EUI;
    connect_params.connection_u.otaa.app_key   = APP_KEY;
    connect_params.connection_u.otaa.nb_trials = 3;

    retcode = lorawan.connect(connect_params);

    if (retcode != LORAWAN_STATUS_OK && retcode != LORAWAN_STATUS_CONNECT_IN_PROGRESS) {
        printf("\r\n Connection error, code = %d \r\n", retcode);
        return -1;
    }

    printf("\r\n Connection - In Progress ...\r\n");
    return 0;
}

void Lora::infiniteLoop()
{
    ev_queue.dispatch_forever();
}

void Lora::onReceive(receive_handler_t callback)
{
    myReceiveHandler = callback;
}

void Lora::setMessageBuilder(message_builder_t callback)
{
    myMessageBuilder = callback;
}
