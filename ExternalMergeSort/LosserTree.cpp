#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <limits>
#include "LosserTree.h"

using namespace std;

const uint64_t FILE_CHUNK_SIZE = 0.75 * 1024 * 1024;

void load_data_to_inputB(const string &fileName, vector<uint32_t> &dataBuffer, uint64_t chunkSizeBytes, uint64_t offset, map<string, uint64_t> &fileDetails)
{

    dataBuffer.clear();
    //  open the file
    ifstream inputFile(fileName, ios::binary);
    if (!inputFile)
    {
        cerr << "Error, unable to open the file " << fileName << endl;
        return;
    }

    inputFile.seekg(0, ios::end);
    uint64_t fileSize = inputFile.tellg();

    uint64_t bytesToRead = chunkSizeBytes;
    if (offset + chunkSizeBytes > fileSize)
    {
        cout << "File size is " << fileSize << "offset is " << offset << endl;
        bytesToRead = fileSize - offset;
    }
    fileDetails[fileName] += bytesToRead;
    cout << "File " << fileName << "offset is " << fileDetails[fileName] << endl;
    size_t numValues = bytesToRead / sizeof(uint32_t);

    dataBuffer.resize(numValues);
    inputFile.seekg(offset, ios::beg);
    inputFile.read(reinterpret_cast<char *>(dataBuffer.data()), bytesToRead);

    if (inputFile)
    {
        cout << "Loaded " << bytesToRead << " bytes into Input Buffer from file. " << offset << "(in losser tree)" << endl;
    }
    else
    {
        cerr << "Error reading the full chunk from the file." << endl;
    }

    inputFile.close();
}
uint64_t getFileSize(const std::string &fileName)
{
    std::ifstream file(fileName, std::ios::binary | std::ios::ate); // Open file in binary mode and position at the end
    if (file.is_open())
    {
        return file.tellg(); // Get the current position (which is the size of the file)
    }
    return 0; // Return 0 if unable to open file
}

string getFileNameForPlayer(uint64_t player)
{
    ostringstream fileNameStream;
    fileNameStream << "sorted_" << player << ".bin";
    return fileNameStream.str();
}

vector<vector<uint32_t>> &LoserTree::getLists()
{
    return lists;
}

vector<uint64_t> &LoserTree::getCurrentIndexes()
{
    return currentIndex;
}

// Constructor to initialize the Loser Tree
LoserTree::LoserTree(vector<vector<uint32_t>> &inputLists, vector<uint64_t> &currentIndx)
{
    k = inputLists.size();
    tree.resize(k, -1);
    lists = inputLists;
    current.resize(k);
    currentIndex = currentIndx;
    for (uint64_t i = 0; i < k; ++i)
    {
        if (!lists[i].empty())
        {
            current[i] = lists[i][0]; // Get the first element from each list
        }
    }
    buildTree();
}

void LoserTree::buildTree()
{

    for (uint64_t i = 0; i < k; ++i)
    {
        playMatch(((k - 1 - i % 2) + i) / 2, i);
    }
}
// Simulate a match between two players (lists) and store the loser in the tree
void LoserTree::playMatch(uint64_t node, uint64_t player)
{
    if (player == -1)
    {
        return;
    }
    if (node == 0)
    {
        if (tree[node] == -1 && player != -1)
        {
            tree[node] = player;
        }
        else if (current[player] <= current[tree[node]])
        {
            winner = player;
        }
        else
        {
            int temp = tree[node];
            tree[node] = player;
            winner = temp;
        }
        return;
    }

    int parent = (node - 1) / 2;
    if (tree[node] == -1 || current[player] >= current[tree[node]])
    {
        int temp = tree[node];
        tree[node] = player;
        playMatch(parent, temp);
    }
    else
    {
        playMatch(parent, player);
    }
}

// Get the current winner (smallest element among lists)
uint32_t LoserTree::getWinner()
{
    return winner; // The root stores the index of the winner
}

void LoserTree::updateTree(uint64_t player, map<string, uint64_t> &fileDetails)
{
    // Move to the next element in the list for the current winner
    currentIndex[player]++;
    // Check if we've exhausted the current chunk for the player
    if (currentIndex[player] >= lists[player].size())
    {
        string fileName = getFileNameForPlayer(player); // Retrieve the file name corresponding to the player

        // Check if there is more data to load from the file
        if (fileDetails[fileName] < getFileSize(fileName))
        {
            // Load the next chunk from the file into the list
            load_data_to_inputB(fileName, lists[player], FILE_CHUNK_SIZE, fileDetails[fileName], fileDetails);
            currentIndex[player] = 0; // Reset index after loading the new chunk
        }
        else
        {
            // No more data to load, set the current value to max to exclude this player
            current[player] = numeric_limits<uint32_t>::max();
        }
    }
    else
    {
        // Update the current value to the next element in the list
        current[player] = lists[player][currentIndex[player]];
    }
    // Rebuild the tree starting from the player
    playMatch((player + (k - 1 - player % 2)) / 2, player);
}
