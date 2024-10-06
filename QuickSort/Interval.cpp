#include "Interval.h"

Interval::Interval(uint32_t a, uint32_t b)
{
    if (a < b)
    {
        min = a;
        max = b;
    }
    else
    {
        min = b;
        max = a;
    }
    isFill = true;
}

Interval::Interval(uint32_t a)
{
    min = a;
    max = a;
    isFill = false;
}