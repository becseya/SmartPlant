/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "aggregation/Manager.hpp"
#include "app/RGBLed.hpp"
#include "app/sensors/Brightness.hpp"
#include "app/sensors/Gps.hpp"
#include "app/sensors/MMA8451Q.hpp"
#include "app/sensors/Si7021.hpp"
#include "app/utils/Json.hpp"
#include "app/utils/LineBufferedSerial.hpp"
#include "app/utils/array.hpp"
#include "app/utils/log.hpp"
#include "app/utils/misc.hpp"
#include "mbed.h"

#include <cstdio>
#include <time.h>

#define MAX_NUMBER_OF_GLOBAL_EVENTS 5
#define SENSOR_UPDATE_PERIOD        2s

#define ID_GREENHOUSE 515
#define ID_ROOM       5
#define ID_ROW        1

using namespace SmartPlant;
using GlobalEventQueue = SmartPlant::EventQueue;

// global event queue
GlobalEventQueue globalEvents(MAX_NUMBER_OF_GLOBAL_EVENTS);

// Hardware elements and other classes
I2C                     i2cBus(PB_9, PB_8);
RGBLed                  rgbLed(PB_12, PA_12, PA_11);
LineBufferedSerial<128> serial(USBTX, USBRX);

// Sensors
Sensors::Brightness sBrightness(PA_4);
Sensors::Gps        sGps(PA_9, PA_10);
Sensors::Si7021     sTempHum(i2cBus);

const auto sensors = make_array<Sensor*>( //
    &sBrightness,
    &sGps,
    &sTempHum);

// Aggregation
const auto aggregators = make_array<Aggregation::Aggregator*>( //
    &sBrightness.aggregator,
    &sTempHum.aggregatorTemp,
    &sTempHum.aggregatorHumidity);

Aggregation::Manager<array_size(aggregators)> aggregationManager(aggregators);

Json::Builder builder;

// -------------------------------------------------- START OF MAIN ---------------------------------------------------

int main()
{
    // init
    LOG_DEBUG("Initializing %u sensors...", sensors.size())
    for (auto& s : sensors) {
        if (!s->init())
            LOG_ERROR("Failed to initialize %s!", s->getName())
    }

    globalEvents.call_every(SENSOR_UPDATE_PERIOD, [&]() -> void {
        for (auto& s : sensors)
            s->update();

        auto tm = sGps.getLastMeasurement().time;

        builder.reset();

        builder.append("timeStamp", mktime(&tm));
        builder.append("greenhouse_id", ID_GREENHOUSE);
        builder.append("room_id", ID_ROOM);
        builder.append("row_id", ID_ROW);
        builder.append("amb_temp", sTempHum.aggregatorTemp.getLastSample());
        builder.append("amb_hum", sTempHum.aggregatorHumidity.getLastSample());
        builder.append("water_temp", 23.2);
        builder.append("pH", 6.3);
        builder.append("EC", 120.3);
        builder.append("light", sBrightness.aggregator.getLastSample());

        printf("%s\n", builder.toString());
    });

    globalEvents.call_every(100ms, [&]() -> void { //
        auto line = serial.readNextLine();

        if (line)
            printf("%s\n", line);
    });

    // infinite loop
    globalEvents.dispatch_forever();
}
