#include "Memory.h"

unsigned char *CPUMemory::get_prg_ram_start() {
    return std::begin(prg_ram);
}
