#ifndef LEAF_H
#define LEAF_H

#include <iostream>
#include <map>
#include <vector>
using namespace std;

class Leaf
{

private:
    uint32_t value;
    vector<uint32_t> buffer;
    string fileName;

    void fillBuffer(const vector<uint32_t> &inputBuffer1, const vector<uint32_t> &inputBuffer2, string fileName);

public:
    Leaf(string fileName);

    void updateLeaf(); // if buffer.size< size/2 fillBuffer using input buffer
};

#endif