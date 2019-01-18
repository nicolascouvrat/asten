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
    uint8_t *get_prg_ram_start(); 
    void debug_dump(std::ostream&, int, int);
private:
    static const int SIZE = 0x10000;
    uint8_t memory[SIZE];
};

#endif
