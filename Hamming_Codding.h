#include "functions.h"

uint64_t CheckOneError(std::vector<bool>& bits, uint64_t block) {
    uint64_t defectBit = 0;
    for (uint64_t j = 0; j < bits.size(); j += block) { //control bits value
        for (uint64_t i = 0; i < block && (i + j) < bits.size(); i++) {
            if (isPowerOfTwo(i + 1)) {
                uint64_t m = bits[i + j];
                bits[i + j] = 0;
                uint64_t sum = 0;
                for (uint64_t number = i; number < block && (number + j) < bits.size(); number += i + 1) {
                    uint64_t k = number;
                    for (number; number < (k + i + 1) && (number + j) < bits.size() && number < block; number++) {;
                        sum ^= bits[number + j];
                    }
                }
                if (m != sum)
                    defectBit += i + 1;
                bits[j + i] = m;
            }
        }
    }
    return (defectBit);
}

void Coding(char* ArcName, uint64_t codingLength) {
    std::vector<bool> bits;

    uint64_t block = 8 * codingLength + (uint64_t) ceil(log2(8 * codingLength + 1));

    std::ifstream Archive(ArcName, std::ios_base::binary);

    if (!Archive.is_open()) {
        std::cout << "File don`t find!";
        return;
    }

    char byte;
    uint64_t index = 1;

    while (Archive.get(byte)) { //insert key-value
        for (int i = 7; i >= 0; i--) {
            if (isPowerOfTwo(index)) {
                bits.push_back(0);
                index++;
            }
            if (isPowerOfTwo(index)) {
                bits.push_back(0);
                index++;
            }
            bits.push_back((byte >> i) & 1);
            index++;
        }
        if (index > 8 * codingLength)
            index = 1;
    }

    for (uint64_t j = 0; j < bits.size(); j += block) { //control bits value
        for (uint64_t i = 0; i < block && i < bits.size(); i++) {
            if (isPowerOfTwo(i + 1)) {
                uint64_t sum = 0;
                for (uint64_t number = i; number < block && number < bits.size(); number += i + 1) {
                    uint64_t k = number;
                    for (; number < k + i + 1 && (number + j) < bits.size() && number < block; number++) {
                        sum ^= bits[number + j];
                    }
                }
                bits[j + i] = sum;
            }
        }
    }

    Archive.close();
    remove(ArcName);

    std::vector<unsigned char> arcBytes;

    int shift = 0;
    while (bits.size() % 8 != 0) {
        bits.push_back(0);
        shift++;
    }

    for (uint64_t i = 0; i < bits.size(); i += 8)
        arcBytes.push_back(GetNumberFrom8Bits(bits, i));

    arcBytes.push_back(shift);

    arcBytes.push_back(codingLength);
    arcBytes.push_back(codingLength >> 8);
    arcBytes.push_back(codingLength >> 16);
    arcBytes.push_back(codingLength >> 24);
    arcBytes.push_back(codingLength >> 32);
    arcBytes.push_back(codingLength >> 40);
    arcBytes.push_back(codingLength >> 48);
    arcBytes.push_back(codingLength >> 56);

    std::ofstream archive;
    archive.open(ArcName, std::ios::out | std::ios::binary);

    archive.write((char*)arcBytes.data(), arcBytes.size());

    archive.close();
}

void Decoding(char* ArcName) {
    std::vector<bool> bits;
    std::vector<unsigned char> Bytes;

    std::ifstream Archive(ArcName, std::ios_base::binary);

    if (!Archive.is_open()) {
        std::cout << "File don`t find! \n";
        return;
    }

    char byte;
    while (Archive.get(byte)) {
        Bytes.push_back(byte);
        for (int i = 7; i >= 0; i--)
            bits.push_back((byte >> i) & 1);
    }

    Archive.close();

    uint64_t codingLength = 0;
    for (uint64_t i = Bytes.size() - 8; i < Bytes.size(); i++)
        codingLength += ((int) Bytes[i] << 8 * (Bytes.size() - i));

    uint64_t block = codingLength * 8 + (uint64_t) ceil(log2(8 * codingLength + 1));

    int shift = Bytes[Bytes.size() - 9];

    for (uint64_t i = 0; i < 72 + shift; i++) //Coding length + shift + shift
        bits.pop_back();

    uint64_t defectBit = CheckOneError(bits, block);
    bits[defectBit - 1] = !bits[defectBit - 1];
    uint64_t defectBitTwo = CheckOneError(bits, block);

    if (defectBit == 0 && defectBitTwo == 0) {
        std::cout << "Successful Decoding! \n";
    } else if (defectBit != 0 && defectBitTwo == 0) {
        std::cout << "One error was found and fixed! \n";
    } else if (defectBit != 0 && defectBitTwo != 0) {
        std::cout << "FATAL ERROR! \n";
        return;
    }

    std::vector<bool> new_bits;

    for (uint64_t l = 0; l < bits.size(); l++)
        if (!isPowerOfTwo(l % block + 1))
            new_bits.push_back(bits[l]);

    std::vector<unsigned char> arcBytes;

    for (uint64_t i = 0; i < new_bits.size(); i += 8) {
        arcBytes.push_back(GetNumberFrom8Bits(new_bits, i)) ;
    }

    std::ofstream archive;
    archive.open(ArcName, std::ios::out | std::ios::binary);

    archive.write((char*)arcBytes.data(), arcBytes.size());

    archive.close();
}