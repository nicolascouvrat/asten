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
    CPUMemory(Console&); 
private:
    Logger log;
    static const int RAM_SIZE = 0x800;
    uint8_t ram[RAM_SIZE];
    Console& console;
};

#endif
