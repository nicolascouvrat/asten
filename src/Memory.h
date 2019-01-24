#ifndef GUARD_MEMORY_H
#define GUARD_MEMORY_H

#include <iterator>
#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>

#include "Utilities.h"
#include "Logger.h"

class Console;
class CPUMemory {
public:
    void debug_dump(uint16_t, uint16_t, uint16_t per_line = 8);
    uint8_t read(uint16_t);
    void write(uint16_t, uint8_t);
    CPUMemory(Console& c): console(c) {}
private:
    static const int SIZE = 0x10000;
    uint8_t memory[SIZE];
    Console& console;
};

#endif
