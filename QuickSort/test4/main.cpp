#include <iostream>
#include <fstream>
#include <vector>
#include <dirent.h> // For directory traversal
#include <cstring>  // For strcmp
#include <string>

void convertBinToTxt(const std::string &binFileName, const std::string &txtFileName)
{
    // Open the binary file for reading
    std::ifstream binFile(binFileName, std::ios::binary);
    if (!binFile)
    {
        std::cerr << "Error opening binary file: " << binFileName << std::endl;
        return;
    }

    // Open the text file for writing
    std::ofstream txtFile(txtFileName);
    if (!txtFile)
    {
        std::cerr << "Error opening text file: " << txtFileName << std::endl;
        return;
    }

    // Read the binary data assuming 32-bit unsigned integers
    uint32_t value;
    while (binFile.read(reinterpret_cast<char *>(&value), sizeof(value)))
    {
        txtFile << value << std::endl; // Write to the text file
    }

    std::cout << "Converted " << binFileName << " to " << txtFileName << std::endl;
}

void convertAllBinFilesToTxt(const std::string &directoryPath)
{
    DIR *dir;
    struct dirent *ent;

    // Open the directory
    if ((dir = opendir(directoryPath.c_str())) != nullptr)
    {
        // Iterate through the files
        while ((ent = readdir(dir)) != nullptr)
        {
            std::string fileName = ent->d_name;

            // Check if file is a .bin file
            if (fileName.length() > 4 && fileName.substr(fileName.length() - 4) == ".bin")
            {
                std::string binFilePath = directoryPath + "/" + fileName;
                std::string txtFilePath = directoryPath + "/" + fileName.substr(0, fileName.length() - 4) + ".txt";

                convertBinToTxt(binFilePath, txtFilePath); // Convert bin to txt
            }
        }
        closedir(dir);
    }
    else
    {
        // Could not open directory
        std::cerr << "Could not open directory: " << directoryPath << std::endl;
    }
}

int main()
{
    std::string folderPath = "./sorted"; // Set your folder path containing .bin files
    convertAllBinFilesToTxt(folderPath);

    return 0;
}
