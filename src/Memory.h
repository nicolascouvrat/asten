#ifndef GUARD_MEMORY_H
#define GUARD_MEMORY_H

#include <iterator>
class CPUMemory {
public:
    static const int RAM_SIZE = 0x800;
    static const int PRG_RAM_SIZE = 0x2000;
    unsigned char *get_prg_ram_start(); 
private:
    unsigned char ram[RAM_SIZE];
    unsigned char prg_ram[PRG_RAM_SIZE];
};

#endif
