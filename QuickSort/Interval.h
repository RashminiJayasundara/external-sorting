#ifndef INTERVAL_H
#define INTERVAL_H

#include <iostream>
using namespace std;

class Interval
{
public:
    uint32_t min;
    uint32_t max;
    bool isFill;

    Interval(uint32_t min, uint32_t max);
    Interval(uint32_t min);
};

#endif