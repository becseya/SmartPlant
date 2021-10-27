#include "misc.hpp"

float getCappedPercentage(unsigned short value, unsigned short min, unsigned short max)
{
    if (value >= max)
        return 100;

    if (value <= min)
        return 0;

    return (float)(value - min) * 100 / (max - min);
}
