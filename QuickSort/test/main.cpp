#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
int main()
{

    std::string binaryFile = "merged.bin"; // Input binary file
    std::string textFile = "output1.txt";  // Output text file

    std::ifstream binFile(binaryFile, std::ios::in | std::ios::binary);
    std::ofstream txtFile(textFile);

    if (!binFile)
    {
        std::cerr << "Error: Cannot open binary file!" << std::endl;
        return 1;
    }
    if (!txtFile)
    {
        std::cerr << "Error: Cannot create text file!" << std::endl;
        return 1;
    }

    // Read data from binary file and write to text file
    std::vector<uint32_t> data;
    uint32_t value;

    // Read each 4-byte value (32-bit unsigned integer)
    while (binFile.read(reinterpret_cast<char *>(&value), sizeof(value)))
    {
        txtFile << value << std::endl;
    }

    binFile.close();
    txtFile.close();

    std::cout << "Binary file converted to text file successfully!" << std::endl;

    return 0;
}
