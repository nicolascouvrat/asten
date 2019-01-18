#ifndef GUARD_CPU_H
#define GUARD_CPU_H

#include "Memory.h"
#include <iostream>
#include <cstdint>

class CPU {
public:
    CPU();
    CPUMemory& get_memory();
private:
    CPUMemory mem;
    uint8_t A, X, Y;                // registers
    uint8_t sp;                     // stack pointer
    uint16_t pc;                    // program counter
    bool C, Z, I, D, B, U, O, N;    // processor flags
};

#endif
