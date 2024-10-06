#include <iostream>
#include <fstream>
#include <random>
#include <cstdint>
#include <sstream>
#include <vector>
#include <chrono>
#include <map>
#include "mergeSort.h"
#include "LosserTree.h"

using namespace std;

void generate_random_numbers(const string &fileName, unsigned int seed)
{

    // set file size as 256MB
    const uint64_t FILE_SIZE_BYTES = 256 * 1024 * 1024;

    // limits the random number generator 1 to million
    const uint32_t MIN_VALUE = 1;
    const uint32_t MAX_VALUE = 1000000;

    // initialize random number generator with seed
    // mersenne twister random generator
    mt19937 generator(seed);
    uniform_int_distribution<uint32_t> distribution(MIN_VALUE, MAX_VALUE);

    // open the file for binary output
    ofstream outputFile(fileName, ios::binary);

    if (!outputFile)
    {
        cerr << "Erro, Unable to Open the File " << fileName << endl;
        return;
    }

    uint64_t fileSize = 0;

    // write random numbers until the file reaches the desired size
    while (fileSize < FILE_SIZE_BYTES)
    {

        // Generate a random number
        uint32_t randomNumber = distribution(generator);

        // write the number in binary format
        outputFile.write(reinterpret_cast<const char *>(&randomNumber), sizeof(randomNumber));

        // update file size
        fileSize += sizeof(randomNumber);
    }

    outputFile.close();
    cout << "File Generated: " << fileName << "(" << FILE_SIZE_BYTES << ")" << endl;
}

void load_data_to_ram(const string &fileName, vector<uint32_t> &dataBuffer, uint64_t chunkSizeBytes, uint64_t offset)
{

    // open the file
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

    size_t numValues = bytesToRead / sizeof(uint32_t);

    dataBuffer.resize(numValues);
    inputFile.seekg(offset, ios::beg);
    inputFile.read(reinterpret_cast<char *>(dataBuffer.data()), bytesToRead);

    if (inputFile)
    {
        cout << "Loaded " << bytesToRead << " bytes into Input Buffer from file. " << offset << endl;
    }
    else
    {
        cerr << "Error reading the full chunk from the file." << endl;
    }

    inputFile.close();
}

void store_sortedData(const string &fileName, vector<uint32_t> &dataBuffer)
{
    // open the file for binary output
    ofstream outputFile(fileName, ios::binary | ios::app);

    if (!outputFile)
    {
        cerr << "Erro, Unable to Open the File " << fileName << endl;
        return;
    }
    for (uint32_t num : dataBuffer)
    {
        // write the number in binary format
        outputFile.write(reinterpret_cast<const char *>(&num), sizeof(num));
    }
    outputFile.close();
    cout << "Sorted File Generated: " << fileName << endl;
}

int main()
{
    string fileName = "random_numbers.bin";
    generate_random_numbers(fileName, 12);
    vector<int> indexOrder;
    const uint64_t RAM_SIZE_BYTES = 16 * 1024 * 1024;
    const uint64_t FILE_SIZE_BYTES = 256 * 1024 * 1024;
    const uint64_t NUM_ALL_EL = FILE_SIZE_BYTES / sizeof(uint32_t);
    const uint64_t NUM_CHUNKS = FILE_SIZE_BYTES / RAM_SIZE_BYTES;
    const uint64_t FILE_CHUNK_SIZE = 0.75 * 1024 * 1024;
    const uint64_t OUTPUT_BUFFER_SIZE = 4 * 1024 * 1024;
    const uint64_t NUM_EL = OUTPUT_BUFFER_SIZE / sizeof(uint32_t);
    map<string, uint64_t> fileDetails;
    auto start = chrono::high_resolution_clock::now();
    MergeSort sorter;
    for (uint64_t chunkIndex = 0; chunkIndex < NUM_CHUNKS; chunkIndex++)
    {
        vector<uint32_t> partOfList;
        uint64_t offset = chunkIndex * RAM_SIZE_BYTES;
        load_data_to_ram(fileName, partOfList, RAM_SIZE_BYTES, offset);
        sorter.mergeSort(partOfList, 0, partOfList.size() - 1);
        ostringstream fileNameStream;
        fileNameStream << "sorted" << "_" << chunkIndex << ".bin";
        string outputFileName = fileNameStream.str();
        fileDetails[outputFileName] = 0;
        store_sortedData(outputFileName, partOfList);
    }
    vector<vector<uint32_t>> inputBuffer;
    vector<uint32_t> outputBuffer;
    for (const auto &pair : fileDetails)
    {
        vector<uint32_t> partOfFile;
        load_data_to_ram(pair.first, partOfFile, FILE_CHUNK_SIZE, pair.second);
        fileDetails[pair.first] = pair.second + FILE_CHUNK_SIZE;
        inputBuffer.push_back(partOfFile);
    }
    vector<uint64_t> currentIndex(16, 0);
    LoserTree loserTree(inputBuffer, currentIndex);
    for (uint64_t i = 0; i < NUM_ALL_EL; i++)
    {
        uint64_t winner = loserTree.getWinner();
        outputBuffer.push_back(loserTree.getLists()[winner][loserTree.getCurrentIndexes()[winner]]);
        if (outputBuffer.size() == NUM_EL)
        {
            ostringstream fileNameStream;
            store_sortedData("merged.bin", outputBuffer);
            outputBuffer.clear();
        }
        loserTree.updateTree(winner, fileDetails);
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Execution time: " << elapsed.count() << " seconds" << endl;

    return 0;
}