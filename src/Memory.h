#ifndef GUARD_MEMORY_H
#define GUARD_MEMORY_H

#include <iterator>
#include <iostream>
#include <vector>
#include "Utilities.h"
#include "Logger.h"

class CPUMemory {
public:
    static const int RAM_SIZE = 0x800;
    static const int PRG_RAM_SIZE = 0x2000;
    unsigned char *get_prg_ram_start(); 
    void debug_dump(std::ostream&, int, int);
private:
    unsigned char ram[RAM_SIZE];
    unsigned char prg_ram[PRG_RAM_SIZE];
};

#endif
