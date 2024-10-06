#ifndef INTERVALHEAP_H
#define INTERVALHEAP_H

#include "Interval.h"
#include <vector>
using namespace std;

class IntervalHeap
{
private:
    vector<Interval> heap;

    void heapifyUp(uint32_t index);
    void heapifyDown(uint32_t index);

public:
    // constructor
    IntervalHeap();

    // destructor to clean up dynamically allocate memory
    ~IntervalHeap();

    // functions to manage interval heap
    void insert(uint32_t value);
    uint32_t getMin();
    uint32_t getMax();
    void deleteMin();
    void deleteMax();
    bool isEmpty();
    int size();
    vector<uint32_t> display();
};

#endif