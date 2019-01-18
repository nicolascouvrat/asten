#include "Memory.h"

unsigned char *CPUMemory::get_prg_ram_start() {
    return std::begin(prg_ram);
}

void CPUMemory::debug_dump(std::ostream& out, int start, int range) {
    Logger log = LoggerStore::getLogger("NESMemory");
    unsigned char *it = prg_ram + start;
    std::vector<HexChar> memory_slice;
    for (int i = 1; i <= range; i++)
        memory_slice.push_back(*it++);
    log << memory_slice << std::endl;

}
