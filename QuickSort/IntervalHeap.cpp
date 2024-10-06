#include <iostream>
#include <vector>
#include <stdexcept>
#include "IntervalHeap.h"

using namespace std;

IntervalHeap::IntervalHeap() { heap = {}; }
IntervalHeap::~IntervalHeap() {}
void IntervalHeap::heapifyUp(uint32_t index)
{
    if (index == 0)
        return;
    int parent = (index - 1) / 2;
    if (heap[index].min < heap[parent].min)
    {
        swap(heap[index].min, heap[parent].min);
        if (!heap[index].isFill)
        {
            heap[index].max = heap[index].min;
        }
    }
    if (heap[index].max > heap[parent].max)
    {
        swap(heap[index].max, heap[parent].max);
        if (!heap[index].isFill)
        {
            heap[index].min = heap[index].max;
        }
    }
    if (heap[index].min > heap[index].max)
        swap(heap[index].min, heap[index].max);
    heapifyUp(parent);
}

void IntervalHeap::insert(uint32_t value)
{
    if (heap.empty())
    {
        heap.push_back(Interval(value));
    }
    else
    {
        if (heap.back().isFill)
        {
            heap.push_back(Interval(value));
            heapifyUp(heap.size() - 1);
        }
        else
        {
            if (heap.back().min <= value)
            {
                heap.back().max = value;
                heap.back().isFill = true;
            }
            else
            {
                heap.back().min = value;
                heap.back().isFill = true;
            }
            heapifyUp(heap.size() - 1);
        }
    }
}

uint32_t IntervalHeap::getMin()
{
    if (heap.empty())
        throw runtime_error("Heap is empty");
    return heap[0].min;
}

uint32_t IntervalHeap::getMax()
{
    if (heap.empty())
        throw runtime_error("Heap is empty");
    return heap[0].max;
}

void IntervalHeap::deleteMax()
{
    if (heap.empty())
        throw runtime_error("Heap is empty");
    if (heap.back().isFill)
    {
        heap[0].max = heap.back().max;
        heap.back().isFill = false;
        heap.back().max = heap.back().min;
        heapifyDown(0);
    }
    else
    {
        heap[0].max = heap.back().max;
        heap.pop_back();
        heapifyDown(0);
    }
}

void IntervalHeap::deleteMin()
{
    if (heap.empty())
        throw runtime_error("Heap is empty");
    if (heap.back().isFill)
    {
        heap[0].min = heap.back().min;
        heap.back().isFill = false;
        heap.back().min = heap.back().max;
        heapifyDown(0);
    }
    else
    {
        heap[0].min = heap.back().min;
        heap.pop_back();
        heapifyDown(0);
    }
}

void IntervalHeap::heapifyDown(uint32_t index)
{
    if (heap[index].min > heap[index].max)
    {
        swap(heap[index].min, heap[index].max);
    }
    int leftChild = 2 * index + 1;
    int rightChild = 2 * index + 2;
    int smallest = index;
    int largest = index;
    if (leftChild < heap.size() && heap[leftChild].min < heap[smallest].min)
    {
        smallest = leftChild;
    }

    if (rightChild < heap.size() && heap[rightChild].min < heap[smallest].min)
    {
        smallest = rightChild;
    }

    if (smallest != index)
    {
        swap(heap[index].min, heap[smallest].min);
        heapifyDown(smallest);
    }

    if (leftChild < heap.size() && heap[leftChild].max > heap[largest].max)
    {
        largest = leftChild;
    }

    if (rightChild < heap.size() && heap[rightChild].max > heap[largest].max)
    {
        largest = rightChild;
    }

    if (largest != index)
    {
        swap(heap[index].max, heap[largest].max);
        heapifyDown(largest);
    }

    if (heap[index].min > heap[index].max)
        swap(heap[index].min, heap[index].max);
}
bool IntervalHeap::isEmpty()
{
    return heap.empty();
}

int IntervalHeap::size()
{
    return heap.size();
}

vector<uint32_t> IntervalHeap::display()
{
    vector<uint32_t> flattenList;
    for (Interval i : heap)
    {

        flattenList.push_back(i.min);
        if (i.isFill)
        {
            flattenList.push_back(i.max);
        }
    }
    return flattenList;
}
