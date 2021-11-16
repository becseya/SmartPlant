#pragma once

namespace SmartPlant {

enum Color
{
    COLOR_RED = 0,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_CLEAR,
    NUMBER_OF_COLORS,
};

const char* colorToString(Color color);

} // namespace SmartPlant
