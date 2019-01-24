#include "Memory.h"
#include "Console.h"
#include "Mapper.h"

/* 
 * CPU MEMORY ORGANIZATION (NES's 6502)
 * $0000
 *     2kb RAM, mirrored 4 times
 * $2000
 *     Access to PPU I/O registers (8 of them, mirrored all accross)
 * $4000
 *     $4000 - $4013: APU register
 *     $4014 ppu OAMDMA register
 *     $4015 APU register
 *     $4016 left joystick (this demands source checking...)
 *     $4017 right joystick (also APU??)
 *     ??? (other stuff maybe?)
 * $5000
 *     Expansion modules
 * $6000
 *     PRG RAM
 * $8000
 *     PRG ROM (lower)
 * $C000
 *     PRG ROM (upper)
 * $10000
 * */


namespace {
    Logger log = Logger::get_logger("NESMemory").set_level(DEBUG);
}

/* DEBUG FUNCTIONS */
void CPUMemory::debug_dump(uint16_t offset, uint16_t range, uint16_t per_line) {
    uint8_t *it = ram + offset;
    log.debug() << "Memory from " <<  hex(offset) << " to " << hex((uint16_t)(offset + range));
    log.toggle_header();
    for (int i = 0; i < range; i++) {
        if (i % per_line == 0) {
            log.debug() << "\n";
            if (i < range - 1)
                log.debug() << std::right << hex((uint16_t)(offset + i)) << ": ";
        }
        log.debug() << hex(read(offset + i));
    }
    log.debug() << "\n";
    log.toggle_header();
}

/* PUBLIC FUNCTIONS */
uint8_t CPUMemory::read(uint16_t address) {
    if (address < 0x2000)
        return ram[address % CPUMemory::RAM_SIZE];
    if (address < 0x6000) {
        // TODO: implement PPU
        log.error() << "UNIMPLEMENTED READ AT " << hex(address) << "\n";
        return -1;
    }
    else
        return console.get_mapper()->read_prg(address); 
}


void CPUMemory::write(uint16_t address, uint8_t value) {
    if (address < 0x2000)
        ram[address % CPUMemory::RAM_SIZE] = value;
    else if (address < 0x6000) {
        // TODO: implement PPU
        log.error() << "UNIMPLEMENTED WRITE AT " << hex(address) << "\n";
    }
    else
        console.get_mapper()->write_prg(address, value);
}
