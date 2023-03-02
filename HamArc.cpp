#include "HamArc.h"

#include <fstream>
#include <iostream>
#include <cstring>

const uint8_t tNameSize = 255;
const uint8_t tMemorySize = 8;

HamArc::HamArc(char* archive_name) {
    arcName = archive_name;

    std::ifstream FILE(arcName, std::ios_base::binary);

    if (!FILE.is_open()) {
        std::ofstream archive;
        archive.open(arcName, std::ios::out | std::ios::binary);
        archive.close();
        return;
    }

    char byte;
    while (FILE.get(byte))
        arcBytes.push_back(byte);

    FILE.close();
}

HamArc::~HamArc() = default;

void HamArc::AppendFile(char* filename) {
    std::ifstream FILE(filename, std::ios_base::binary);

    if (!FILE.is_open()) {
        std::cout << filename << " could not be opened \n";
        return;
    }

    for (int i = 0; i < 255; i++) {
        if (strlen(filename) > i)
            arcBytes.push_back(filename[i]);
        else {
            arcBytes.push_back(0);
        }
    }

    FILE.seekg(0, std::ios::end);
    uint64_t size = FILE.tellg();
    FILE.seekg(0, std::ios::beg);

    arcBytes.push_back(size);
    arcBytes.push_back(size >> 8);
    arcBytes.push_back(size >> 16);
    arcBytes.push_back(size >> 24);
    arcBytes.push_back(size >> 32);
    arcBytes.push_back(size >> 40);
    arcBytes.push_back(size >> 48);
    arcBytes.push_back(size >> 56);

    char byte;
    while(FILE.get(byte))
        arcBytes.push_back(byte);

    FILE.close();
    remove(filename);

    std::ofstream archive;
    archive.open(arcName, std::ios::out | std::ios::binary);

    archive.write((char*)arcBytes.data(), arcBytes.size()); //Creating archive file

    archive.close();
}

void HamArc::DeleteFile(char* filename) {
    uint64_t index = 0;
    std::vector<unsigned char> tmp_vec;
    while(true) {
        if (index >= arcBytes.size()) {
            std::cout << "File not fount! :( \n";
            break;
        }

        std::string tmp_str;
        for (int i = index; i < index + tNameSize; i++)
            if (arcBytes[i] != 0)
                tmp_str += arcBytes[i];

        index += tNameSize;
        uint64_t tmp_size = 0;

        for (uint64_t i = index; i < index + tMemorySize; i++)
            tmp_size += ((int)arcBytes[i] << 8 * (i - index));

        index += tMemorySize;

        if (tmp_str == filename) {
            uint64_t arcSize = arcBytes.size();
            for (uint64_t i = 0; i < index - tMemorySize - tNameSize; i++)
                tmp_vec.push_back(arcBytes[i]);
            for (uint64_t i = index + tmp_size; i < arcBytes.size(); i++)
                tmp_vec.push_back(arcBytes[i]);
            for (uint64_t i = 0; i < tmp_vec.size(); i++)
                arcBytes[i] = tmp_vec[i];
            for (uint64_t i = 0; i < arcSize - tmp_vec.size(); i++) {
                arcBytes.pop_back();
            }
            break;
        } else {
            index += tmp_size;
        }
    }
    std::ofstream archive;
    archive.open(arcName, std::ios::out | std::ios::binary);

    archive.write((char*)arcBytes.data(), arcBytes.size()); //Creating archive file

    archive.close();
}

void HamArc::ExtractFile(char* filename) {
    uint64_t index = 0;
    std::vector<unsigned char> tmp_vec;
    while(true) {
        if (index >= arcBytes.size()) {
            std::cout << "File not found! :( \n";
            break;
        }
        std::string tmp_str;
        for (int i = index; i < index + tNameSize; i++)
            if (arcBytes[i] != 0)
                tmp_str += arcBytes[i];

        index += tNameSize;
        uint64_t tmp_size = 0;

        for (uint64_t i = index; i < index + tMemorySize; i++)
            tmp_size += ((int)arcBytes[i] << 8 * (i - index));

        index += tMemorySize;

        if (tmp_str == filename) {
            for (uint64_t i = index; i < index + tmp_size; i++)
                tmp_vec.push_back(arcBytes[i]);
            break;
        } else {
            index += tmp_size;
        }
    }

    DeleteFile(filename);
    std::ofstream archive;
    archive.open(filename, std::ios::out | std::ios::binary);

    archive.write((char*)tmp_vec.data(), tmp_vec.size());

    archive.close();
}

void HamArc::ShowFiles() {
    uint64_t index = 0;
    std::vector<unsigned char> tmp_vec;

    while(true) {
        if (index >= arcBytes.size()) {
            for (int i = 0; i < fileNames.size(); i++)
                std::cout << fileNames[i] << " ";
            if (arcBytes.size() == 0)
                std::cout << "Archive is empty! :( \n";
            break;
        }

        std::string tmp_str;
        for (int i = index; i < index + tNameSize; i++)
            if (arcBytes[i] != 0)
                tmp_str += arcBytes[i];

        fileNames.push_back(tmp_str);
        index += tNameSize;
        uint64_t tmp_size = 0;

        for (uint64_t i = index; i < index + tMemorySize; i++)
            tmp_size += ((int)arcBytes[i] << 8 * (i - index));

        index += tMemorySize;

        index += tmp_size;
    }
}

void HamArc::ExtractAll() {
    uint64_t index = 0;
    std::vector<unsigned char> tmp_vec;
    while (true) {
        if (index >= arcBytes.size()) {
            std::cout << "Extract all done \n";
            break;
        }
        std::string tmp_str;
        for (int i = index; i < index + tNameSize; i++)
            if (arcBytes[i] != 0)
                tmp_str += arcBytes[i];

        index = 0;

        char* filename = const_cast<char*>(tmp_str.c_str());

        ExtractFile(filename);
    }

}



