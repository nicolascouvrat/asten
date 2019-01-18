#include "Memory.h"

/* 
 * CPU MEMORY ORGANIZATION (6502)
 * Works in pages of 256 bytes.
 * The first page ($0000 - $00ff) is the Zero page.
 * The second page ($0100 - $01ff) is reserved for the stack.
 * The last 6 bytes ($fffa - $ffff) are reserved.
 * */

uint8_t *CPUMemory::get_prg_ram_start() {
    return memory + 0x6000;
}

void CPUMemory::debug_dump(std::ostream& out, int offset, int range) {
    Logger log = LoggerStore::getLogger("NESMemory");
    uint8_t *it = memory + offset;
    std::vector<HexChar> memory_slice;
    for (int i = 1; i <= range; i++)
        memory_slice.push_back(*it++);
    log << memory_slice << std::endl;

}

