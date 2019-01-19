#include "Memory.h"
#include <iomanip>

/* 
 * CPU MEMORY ORGANIZATION (6502)
 * Works in pages of 256 bytes.
 * The first page ($0000 - $00ff) is the Zero page.
 * The second page ($0100 - $01ff) is reserved for the stack.
 * The last 6 bytes ($fffa - $ffff) are reserved.
 * */


namespace {
    Logger log = Logger::get_logger("NESMemory").set_level(DEBUG);
}

/* DEBUG FUNCTIONS */
void CPUMemory::debug_dump(int offset, int range, int per_line) {
    uint8_t *it = memory + offset;
    log.debug() << "Memory from " << std::hex << offset << " to " << offset + range << "\n";
    log.toggle_header();
    for (int i = 1; i <= range; i++) {
        log.debug() << std::setw(5) << hex(*it++);
        if (i % per_line == 0)
            log.debug() << "\n";
    }
    log.toggle_header();
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
