#include "Memory.h"
#include "Console.h"
#include "Mapper.h"

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
void CPUMemory::debug_dump(uint16_t offset, uint16_t range, uint16_t per_line) {
    uint8_t *it = memory + offset;
    log.debug() << "Memory from " <<  hex(offset) << " to " << hex((uint16_t)(offset + range));
    log.toggle_header();
    for (int i = 0; i < range; i++) {
        if (i % per_line == 0) {
            log.debug() << "\n";
            if (i < range - 1)
                log.debug() << std::right << hex((uint16_t)(offset + i)) << ": ";
        }
        log.debug() << hex(*it++);
    }
    log.debug() << "\n";
    log.toggle_header();
}

/* PUBLIC FUNCTIONS */
uint8_t CPUMemory::read(uint16_t address) {
    if (address < 0x6000)
        return memory[address]; 
    else
        return console.get_mapper()->read_prg(address); 
    }


void CPUMemory::write(uint16_t address, uint8_t value) {
    if (address < 0x6000)
        memory[address] = value;
    else
        console.get_mapper()->write_prg(address, value);
}
