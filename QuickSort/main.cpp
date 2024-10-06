#include <iostream>
#include <fstream>
#include <algorithm>
#include <regex>
#include <sstream>
#include <chrono>
#include <cstdint>
#include "IntervalHeap.h"
#include "mergeSort.h"

vector<string> fileNames;
void load_data_to_input_buffer(const string &fileName, vector<uint32_t> &dataBuffer, uint64_t chunkSizeBytes, uint64_t offset)
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
void store_Data(const string &fileName, vector<uint32_t> &dataBuffer)
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
void add_data_to_heap(vector<uint32_t> &dataBuffer, uint32_t &min, uint32_t &max, vector<uint32_t> &smallBuffer, vector<uint32_t> &largeBuffer, size_t noOfElements, IntervalHeap &ih, uint32_t threshold, string &smallFileName, string &largeFileName)
{

    for (uint32_t num : dataBuffer)
    {
        if (num < min)
        {
            // cout << "Add small buffer " << num << endl;
            smallBuffer.push_back(num);
            // add to small buffer
        }
        else if (num > max)
        {
            // add to large buffer
            // cout << "Add to large buffer" << num << endl;
            largeBuffer.push_back(num);
        }
        else
        {
            // Determine whether the number is close to `min` or `max`
            if (abs((int)num - (int)min) <= threshold)
            {
                // If close to `min`, remove `min` and add `num` to heap
                smallBuffer.push_back(min);
                ih.deleteMin();
                ih.insert(num);
                min = ih.getMin(); // Update `min` from heap
            }
            else if (abs((int)num - (int)max) <= threshold)
            {
                // If close to `max`, remove `max` and add `num` to heap
                largeBuffer.push_back(max);
                ih.deleteMax();
                ih.insert(num);
                max = ih.getMax(); // Update `max` from heap
            }
            else
            {
                // cout << "Min change " << num << endl;
                //  exist min add to small buffer
                smallBuffer.push_back(min);
                ih.deleteMin();
                ih.insert(num);
                min = ih.getMin();
            }
        }
        if (smallBuffer.size() == noOfElements)
        {
            // cout << "Small Buffer fill and copy to memory" << endl;
            store_Data(smallFileName, smallBuffer);
            smallBuffer.clear();
        }
        if (largeBuffer.size() == noOfElements)
        {
            // cout << "Large Buffer fill and copy to memory" << endl;
            store_Data(largeFileName, largeBuffer);
            largeBuffer.clear();
        }
    }
}

void processFileRecursively(const string &fileName, int itr_no, size_t numElements, uint64_t INPUT_BUFFER, uint64_t MIDLLE, int NUM_CHUNKS)
{
    MergeSort sorter;
    IntervalHeap ih;
    vector<uint32_t> partOfList_1;
    vector<uint32_t> smallBuffer;
    vector<uint32_t> largeBuffer;

    load_data_to_input_buffer(fileName, partOfList_1, MIDLLE, 0); // fist fill middle group

    for (uint32_t num : partOfList_1)
    {
        ih.insert(num);
    }

    uint32_t min = ih.getMin();
    uint32_t max = ih.getMax();
    uint64_t offset = MIDLLE;
    uint32_t threshold = (max - min) * 0.05; // 5% of the current range
    string smallFileName = "small_" + to_string(itr_no) + "_" + to_string(rand()) + ".bin";
    string largeFileName = "large_" + to_string(itr_no) + "_" + to_string(rand()) + ".bin";
    for (uint64_t chunkIndex = 1; chunkIndex <= NUM_CHUNKS; chunkIndex++)
    {
        vector<uint32_t> partOfList_2;
        load_data_to_input_buffer(fileName, partOfList_2, INPUT_BUFFER, offset);
        add_data_to_heap(partOfList_2, min, max, smallBuffer, largeBuffer, numElements, ih, threshold, smallFileName, largeFileName);
        offset += INPUT_BUFFER;
    }

    vector<uint32_t> flattenList = ih.display();
    uint32_t min_for_file_name = ih.getMin();
    sorter.mergeSort(flattenList, 0, flattenList.size() - 1); // sort lastly
    string sorted = "sorted_middle_" + to_string(min_for_file_name) + ".bin";
    fileNames.push_back(sorted);
    store_Data(sorted, flattenList);

    // Write any remaining data from small and large buffers
    if (!smallBuffer.empty())
    {
        store_Data(smallFileName, smallBuffer);
    }
    if (!largeBuffer.empty())
    {
        store_Data(largeFileName, largeBuffer);
    }

    // Process small file if it has data
    ifstream smallFile(smallFileName, ios::binary | ios::ate);
    if (smallFile.tellg() > 0)
    {
        uint64_t fileSize = smallFile.tellg();
        if (fileSize >= MIDLLE)
        {
            fileSize -= MIDLLE;
        }
        else
        {
            fileSize = 0;
        }
        smallFile.close();
        processFileRecursively(smallFileName, itr_no + 1, numElements, INPUT_BUFFER, MIDLLE, fileSize / INPUT_BUFFER);
    }

    // Process large file if it has data
    ifstream largeFile(largeFileName, ios::binary | ios::ate);
    if (largeFile.tellg() > 0)
    {
        uint64_t fileSize = largeFile.tellg();
        if (fileSize >= MIDLLE)
        {
            fileSize -= MIDLLE;
        }
        else
        {
            fileSize = 0;
        }
        largeFile.close();
        processFileRecursively(largeFileName, itr_no, numElements, INPUT_BUFFER, MIDLLE, fileSize / INPUT_BUFFER);
    }
}

int extractNumberFromFilename(const string &filename)
{
    regex re(R"(sorted_middle_(\d+)\.bin)");
    smatch match;

    if (regex_search(filename, match, re))
    {
        return stoi(match[1]); // Extract and convert the number to an integer
    }

    throw runtime_error("Invalid filename format: " + filename);
}

// Function to merge the contents of multiple sorted files into one file
void mergeFiles(const vector<string> &sortedFilenames, const string &outputFile)
{
    ofstream output(outputFile, ios::binary);

    if (!output)
    {
        cerr << "Failed to open output file: " << outputFile << endl;
        return;
    }

    for (const string &filename : sortedFilenames)
    {
        ifstream input(filename, ios::binary);

        if (!input)
        {
            cerr << "Failed to open input file: " << filename << endl;
            continue;
        }

        // Copy the content of the current file to the output file
        output << input.rdbuf();

        input.close();
    }

    output.close();
    cout << "Files merged successfully into " << outputFile << endl;
}

int main()
{

    string fileName = "random_numbers.bin";

    const uint64_t SMALL = 2 * 1024 * 1024;
    const uint64_t LARGE = 2 * 1024 * 1024;
    const uint64_t MIDLLE = 8 * 1024 * 1024; // 8 * 1024 * 1024;
    const uint64_t INPUT_BUFFER = 4 * 1024 * 1024;
    const uint64_t FILE_SIZE_BYTES = 256 * 1024 * 1024;
    const int NUM_CHUNKS = (FILE_SIZE_BYTES - MIDLLE) / INPUT_BUFFER;
    // Calculate number of uint32_t elements that can fit in 2MB
    size_t numElements = 2 * 1024 * 1024 / sizeof(uint32_t);
    auto start = chrono::high_resolution_clock::now();
    // Start recursive processing with the initial file and iteration number 0
    processFileRecursively(fileName, 0, numElements, INPUT_BUFFER, MIDLLE, NUM_CHUNKS);

    // Sort filenames based on the numeric value in the filename
    sort(fileNames.begin(), fileNames.end(), [](const string &a, const string &b)
         { return extractNumberFromFilename(a) < extractNumberFromFilename(b); });

    // Merge the sorted files into a single output file
    string outputFile = "merged_sorted_files.bin";
    mergeFiles(fileNames, outputFile);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Execution time: " << elapsed.count() << " seconds" << endl;
    return 0;
}