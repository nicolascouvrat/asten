#include "Cpu.h"

int CPU::read_id() {
    return id;
}

CPU::CPU(): id(1) {}
