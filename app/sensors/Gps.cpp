#include "Gps.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

#define CHX_SEPARATOR   '*'
#define FIELD_SEPARATOR ','

#define NEXT_FIELD_OR_RETURN(field)                                                                                    \
    field = nextField(field);                                                                                          \
    if (!field)                                                                                                        \
        return false;

// --------------------------------------------------------------------------------------------------------------------

using namespace SmartPlant::Sensors;

Gps::Gps(PinName tx, PinName rx)
    : LineBufferedSerial<128>(tx, rx)
    , Sensor("GPS")
{}

bool Gps::init()
{
    uart.set_baud(9600);
    uart.set_format(8, BufferedSerial::None, 1);

    return true;
}

void Gps::update()
{
    const char* line;
    GpsInfo     newInfo;

    while ((line = readNextLine())) {

        if (!checkLine(line))
            continue;

        if (!parseLine(line, &newInfo))
            continue;

        lastInfo = newInfo;
    }

    LOG_SENSOR("lat: %f, lon: %f %d-%02d-%02d %02d:%02d:%02d %c",
               lastInfo.lat,
               lastInfo.lon,
               lastInfo.time.tm_year + 1900,
               lastInfo.time.tm_mon + 1,
               lastInfo.time.tm_mday,
               lastInfo.time.tm_hour,
               lastInfo.time.tm_min,
               lastInfo.time.tm_sec,
               lastInfo.valid ? ' ' : '!');
}

bool Gps::parseLine(const char* line, GpsInfo* info)
{
    if (!line)
        return false;

    if (strstr(line, "$GPRMC")) {
        const char* field = line;

        // time
        NEXT_FIELD_OR_RETURN(field);
        int time           = atoi(field);
        info->time.tm_sec  = time % 100;
        time               = time / 100;
        info->time.tm_min  = time % 100;
        info->time.tm_hour = time / 100;

        // status
        NEXT_FIELD_OR_RETURN(field);
        info->valid = (field[0] == 'A');

        // latitude
        NEXT_FIELD_OR_RETURN(field);
        float latitude_raw = atof(field);
        float latitude     = (int)(latitude_raw / 100);
        latitude_raw -= latitude * 100;
        latitude += latitude_raw / 60.0f;
        NEXT_FIELD_OR_RETURN(field);
        info->lat = (field[0] == 'N') ? latitude : -latitude;

        // longitude
        NEXT_FIELD_OR_RETURN(field);
        float longitude_raw = atof(field);
        float longitude     = (int)(longitude_raw / 100);
        longitude_raw -= longitude * 100;
        longitude += longitude_raw / 60.0f;
        NEXT_FIELD_OR_RETURN(field);
        info->lon = (field[0] == 'E') ? longitude : -longitude;

        // speed
        NEXT_FIELD_OR_RETURN(field);
        info->speed = atof(field);

        // heading
        NEXT_FIELD_OR_RETURN(field);
        info->heading = atof(field);

        // date
        NEXT_FIELD_OR_RETURN(field);
        int date           = atoi(field);
        info->time.tm_year = 100 + date % 100;
        date               = date / 100;
        info->time.tm_mon  = date % 100 - 1;
        info->time.tm_mday = date / 100;

        return true;
    }

    return false;
}

bool Gps::checkLine(const char* line)
{
    size_t   len = strlen(line);
    unsigned chx;

    if (len < 4)
        return false;

    if (line[len - 3] != CHX_SEPARATOR)
        return false;

    if (sscanf(&line[len - 2], "%x", &chx) != 1)
        return false;

    return ((uint8_t)chx == calculateChx(line));
}

uint8_t Gps::calculateChx(const char* line)
{
    uint8_t sum = 0;

    line++; // skip $

    while (*line && (*line != CHX_SEPARATOR))
        sum ^= *line++;

    return sum;
}

const char* Gps::nextField(const char* ptr)
{
    while (*ptr && *ptr != FIELD_SEPARATOR)
        ptr++;

    if (*ptr) // skip FIELD_SEPARATOR unless we are at the end
        ptr++;

    return ptr;
}
