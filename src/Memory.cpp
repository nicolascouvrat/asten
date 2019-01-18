#include "Memory.h"

/* 
 * CPU MEMORY ORGANIZATION (6502)
 * Works in pages of 256 bytes.
 * The first page ($0000 - $00ff) is the Zero page.
 * The second page ($0100 - $01ff) is reserved for the stack.
 * The last 6 bytes ($fffa - $ffff) are reserved.
 * */


/* DEBUG FUNCTIONS */
void CPUMemory::debug_dump(std::ostream& out, int offset, int range) {
    Logger log = LoggerStore::getLogger("NESMemory");
    uint8_t *it = memory + offset;
    std::vector<HexChar> memory_slice;
    for (int i = 1; i <= range; i++)
        memory_slice.push_back(*it++);
    log << memory_slice << std::endl;

}

/* PUBLIC FUNCTIONS */
uint8_t CPUMemory::read(uint16_t address) { return memory[address]; }

void CPUMemory::write(uint16_t address, uint8_t value) { memory[address] = value; }

void CPUMemory::load(uint16_t address, const std::vector<uint8_t>& values) {
    std::vector<uint8_t>::const_iterator it;
    uint16_t i = address;
    for (it = values.begin(); it != values.end(); it++)
        memory[i++] = *it;
}
