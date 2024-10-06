#ifndef LOSSERTREE_H
#define LOSSERTREE_H
#include <iostream>
#include <vector>
#include <limits>
using namespace std;

class LoserTree
{
private:
    uint64_t k;
    uint64_t winner;                // Number of players (input lists)
    vector<uint64_t> tree;          // The tree that keeps track of losers
    vector<vector<uint32_t>> lists; // Input sorted lists
    vector<uint32_t> current;       // Current value for each list
    vector<uint64_t> currentIndex;

public:
    LoserTree(vector<vector<uint32_t>> &inputLists, vector<uint64_t> &currentIndex);
    void buildTree();                                                     // Initialize the tree
    void playMatch(uint64_t node, uint64_t player);                       // Simulate a match between tree nodes
    uint32_t getWinner();                                                 // Get the current winner (smallest value)
    void updateTree(uint64_t player, map<string, uint64_t> &fileDetails); // Update the tree after the winner is replaced
    vector<vector<uint32_t>> &getLists();
    vector<uint64_t> &getCurrentIndexes();
};

#endif