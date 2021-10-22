#pragma once

#include <cstdio>

#define ___LOG_LEVEL_ALWAYS 0
#define ___LOG_LEVEL_NORMAL 1
#define ___LOG_LEVEL_DEBUG  2

#define ___LOG_CURRENT_LOG_LEVEL ___LOG_LEVEL_DEBUG

#define ___LOG(level, ...)                                                                                             \
    {                                                                                                                  \
        if (level <= ___LOG_CURRENT_LOG_LEVEL) {                                                                       \
            printf(__VA_ARGS__);                                                                                       \
            printf("\n");                                                                                              \
        }                                                                                                              \
    }

// --------------------------------------------------------------------------------------------------------------------

#define LOG(...)       ___LOG(___LOG_LEVEL_NORMAL, __VA_ARGS__)
#define LOG_DEBUG(...) ___LOG(___LOG_LEVEL_DEBUG, __VA_ARGS__)
