#pragma once

#include <cstdio>

#define ___LOG_LEVEL_ALWAYS 0
#define ___LOG_LEVEL_NORMAL 1
#define ___LOG_LEVEL_DEBUG  2

#define ___LOG_CURRENT_LOG_LEVEL ___LOG_LEVEL_DEBUG

#define __LOG_NL "\r\n"

#define ___LOG(level, ...)                                                                                             \
    {                                                                                                                  \
        if (level <= ___LOG_CURRENT_LOG_LEVEL) {                                                                       \
            printf(__VA_ARGS__);                                                                                       \
            printf(__LOG_NL);                                                                                          \
        }                                                                                                              \
    }

// --------------------------------------------------------------------------------------------------------------------

#define LOG(...)       ___LOG(___LOG_LEVEL_NORMAL, __VA_ARGS__)
#define LOG_DEBUG(...) ___LOG(___LOG_LEVEL_DEBUG, __VA_ARGS__)
#define LOG_ERROR(...) ___LOG(___LOG_LEVEL_ALWAYS, "ERROR " __VA_ARGS__)

#define LOG_DEBUG_BUFFER(buffer, size, ...)                                                                            \
    {                                                                                                                  \
        printf(__VA_ARGS__);                                                                                           \
        for (uint8_t i = 0; i < size; i++) {                                                                           \
            printf("%02x ", buffer[i]);                                                                                \
        }                                                                                                              \
        printf(" (%d bytes)" __LOG_NL, size);                                                                          \
    }
