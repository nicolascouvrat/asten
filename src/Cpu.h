#ifndef GUARD_CPU_H
#define GUARD_CPU_H

#include "Memory.h"
#include <iostream>

class CPU {
public:
    CPU();
    CPUMemory& get_memory();
private:
    CPUMemory mem;
};

#endif
