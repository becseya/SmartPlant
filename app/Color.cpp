#include "Color.hpp"

const char* SmartPlant::colorToString(Color color)
{
    switch (color) {
        case COLOR_RED: return "RED";
        case COLOR_GREEN: return "GREEN";
        case COLOR_BLUE: return "BLUE";
        case COLOR_CLEAR: return "CLEAR";
        default: return "null";
    }
}
