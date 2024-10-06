#ifndef MERGESORT_H
#define MERGESORT_H
#include <vector>

using namespace std;
class MergeSort
{
private:
    void merge(vector<uint32_t> &arr, int left, int mid, int right);

public:
    MergeSort();
    ~MergeSort();

    void mergeSort(vector<uint32_t> &arr, int left, int right);
};

#endif
