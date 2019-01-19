#ifndef GUARD_MEMORY_H
#define GUARD_MEMORY_H

#include <iterator>
#include <iostream>
#include <vector>
#include <cstdint>

#include "Utilities.h"
#include "Logger.h"

class CPUMemory {
public:
    void debug_dump(int, int, int per_line = 8);
    uint8_t read(uint16_t);
    void write(uint16_t, uint8_t);
    void load(uint16_t, const std::vector<uint8_t>&);
private:
    static const int SIZE = 0x10000;
    uint8_t memory[SIZE];
};

#endif
