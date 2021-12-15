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
#include "../utils/log.hpp"
#include "lora.hpp"
#include "lora_radio_helper.h"

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

#define LOG_ERROR_AND_RETURN(...)                                                                                      \
    {                                                                                                                  \
        LOG_ERROR(__VA_ARGS__);                                                                                        \
        return -1;                                                                                                     \
    }

/**
 * This event queue is the global event queue for both the
 * application and stack. To conserve memory, the stack is designed to run
 * in the same thread as the application and the application is responsible for
 * providing an event queue to the stack that will be used for ISR deferment as
 * well as application information event queuing.
 */
static EventQueue* ev_queue;

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
        LOG_ERROR("Failed to build message");
        return;
    }

    packet_len = retcode;

    retcode = lorawan.send(MBED_CONF_LORA_APP_PORT, tx_buffer, packet_len, MSG_UNCONFIRMED_FLAG);

    if (retcode < 0) {
        if (retcode == LORAWAN_STATUS_WOULD_BLOCK) {
            LOG_DEBUG("TX would block. Reattempting...");
            ev_queue->call_in(3000, send_message); // retry in 3 seconds
        } else
            LOG_ERROR("TX failed (code %d)", retcode);

        return;
    }

    LOG_DEBUG("%d bytes scheduled for transmission", retcode);
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
        LOG_ERROR("RX failed. (code %d)", retcode);
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
            LOG("Connection - Successful");
            send_message();
            break;
        case DISCONNECTED: //
            LOG("Disconnected Successfully");
            break;
        case TX_DONE:
            LOG_DEBUG("Message Sent to Network Server");
            send_message();
            break;
        case TX_TIMEOUT:
        case TX_ERROR:
        case TX_CRYPTO_ERROR:
        case TX_SCHEDULING_ERROR:
            LOG_ERROR("TX failed. EventCode = %d", event);
            // try again
            send_message();
            break;
        case RX_DONE:
            LOG_DEBUG("Received message from Network Server");
            receive_message();
            break;
        case RX_TIMEOUT:
        case RX_ERROR: //
            LOG_ERROR("RX failed. EventCode = %d", event);
            break;
        case JOIN_FAILURE: //
            LOG_ERROR("OTAA Failed - Check Keys");
            break;
        case UPLINK_REQUIRED:
            LOG_DEBUG("Uplink required by NS");
            send_message();
            break;
        default: //
            MBED_ASSERT("Unknown Event");
    }
}

// --------------------------------------------------------------------------------------------------------------------

int Lora::init(EventQueue& eventQueue)
{
    ev_queue = &eventQueue;

    // stores the status of a call to LoRaWAN protocol
    lorawan_status_t retcode;

    // Initialize LoRaWAN stack
    if (lorawan.initialize(ev_queue) != LORAWAN_STATUS_OK)
        LOG_ERROR_AND_RETURN("LoRa initialization failed!");

    LOG_DEBUG("Mbed LoRaWANStack initialized");

    // prepare application callbacks
    callbacks.events = mbed::callback(lora_event_handler);
    lorawan.add_app_callbacks(&callbacks);

    // Set number of retries in case of CONFIRMED messages
    if (lorawan.set_confirmed_msg_retries(CONFIRMED_MSG_RETRY_COUNTER) != LORAWAN_STATUS_OK)
        LOG_ERROR_AND_RETURN("set_confirmed_msg_retries failed!");

    LOG_DEBUG("CONFIRMED message retries : %d ", CONFIRMED_MSG_RETRY_COUNTER);

    // Enable adaptive data rate
    if (lorawan.enable_adaptive_datarate() != LORAWAN_STATUS_OK)
        LOG_ERROR_AND_RETURN("enable_adaptive_datarate failed!");

    LOG_DEBUG("Adaptive data  rate (ADR) - Enabled ");
    lorawan_connect_t connect_params;
    connect_params.connect_type                = LORAWAN_CONNECTION_OTAA;
    connect_params.connection_u.otaa.dev_eui   = DEV_EUI;
    connect_params.connection_u.otaa.app_eui   = APP_EUI;
    connect_params.connection_u.otaa.app_key   = APP_KEY;
    connect_params.connection_u.otaa.nb_trials = 3;

    retcode = lorawan.connect(connect_params);

    if (retcode != LORAWAN_STATUS_OK && retcode != LORAWAN_STATUS_CONNECT_IN_PROGRESS)
        LOG_ERROR_AND_RETURN("Connection error, code = %d", retcode);

    LOG_DEBUG("Connection - In Progress ...");
    return 0;
}

void Lora::onReceive(receive_handler_t callback)
{
    myReceiveHandler = callback;
}

void Lora::setMessageBuilder(message_builder_t callback)
{
    myMessageBuilder = callback;
}
