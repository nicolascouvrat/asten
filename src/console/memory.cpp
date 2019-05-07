#include "memory.h"

// TODO: remove
#include <iostream>

#include "console.h"
#include "mapper.h"

/* 
 * CPU MEMORY ORGANIZATION (NES's 6502)
 * $0000
 *     2kb RAM, mirrored 4 times
 * $2000 *     Access to PPU I/O registers (8 of them, mirrored all accross)
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
 *
 * PPU MEMORY ORGANIZATION
 * $0000
 *     Pattern table (256 * 8 * 2) * 2 (In cartridge)
 * $2000
 *     (Name table (30 * 32) + Attribut table (64)) * 4
 * $3000
 *     Empty
 * $3F00
 *     Image Palette
 * $3F10
 *     Sprite Palette
 * $3F20
 *     Empty
 * $4000
 **/


Memory::Memory(Console& c, Logger l):
  log(l), console(c)
{
  log.setLevel(DEBUG);
}

CPUMemory::CPUMemory(Console& c):
  Memory(c, Logger::getLogger("CPUMemory"))
{}

PPUMemory::PPUMemory(Console& c):
  Memory(c, Logger::getLogger("PPUMemory"))
{}

/* DEBUG FUNCTIONS */
void Memory::debugDump(uint16_t offset, uint16_t range, uint16_t perLine) {
  log.debug() << "Memory from " <<  hex(offset) << " to " << hex((uint16_t)(offset + range));
  log.toggleHeader();
  for (int i = 0; i < range; i++) {
    if (i % perLine == 0) {
      log.debug() << "\n";
      if (i < range - 1)
        log.debug() << std::right << hex((uint16_t)(offset + i)) << ": ";
    }
    log.debug() << hex(read(offset + i));
  }
  log.debug() << "\n";
  log.toggleHeader();
}

/* PUBLIC FUNCTIONS */
uint8_t CPUMemory::read(uint16_t address) {
  if (address < 0x2000)
    return ram[address % CPUMemory::RAM_SIZE];
  else if (address < 0x4000)
    return console.getPpu().readRegister(0x2000 + address % 8);
  else if (address == 0x4014) {
    return console.getPpu().readRegister(address);
  }
  else if (address == 0x4015) {
    // TODO: APU
    log.error() << "UNIMPLEMENTED READ AT " << hex(address) << "\n";
    return 0;
  }
  else if (address == 0x4016)
    return console.getLeftController().read();
  else if (address == 0x4017)
    return console.getRightController().read();
  if (address < 0x6000) {
    // TODO: expansion modules and other stuff
    log.error() << "UNIMPLEMENTED READ AT " << hex(address) << "\n";
    return 0;
  }
  else
    return console.getMapper()->readPrg(address); 
}


void CPUMemory::write(uint16_t address, uint8_t value) {
  if (address < 0x2000)
    ram[address % CPUMemory::RAM_SIZE] = value;
  else if (address < 0x4000)
    console.getPpu().writeRegister(0x2000 + address % 8, value);
  else if (address == 0x4014)
    console.getPpu().writeRegister(address, value);
  else if (address == 0x4015) {
    // TODO: APU
    log.error() << "UNIMPLEMENTED WRITE AT " << hex(address) << "\n";
  }
  else if (address == 0x4016)
    console.getLeftController().write(value);
  else if (address == 0x4017)
    console.getRightController().write(value);
  else if (address < 0x6000) {
    // TODO: implement expansion modules
    log.error() << "UNIMPLEMENTED WRITE AT " << hex(address) << "\n";
  }
  else
    console.getMapper()->writePrg(address, value);
}

uint8_t PPUMemory::read(uint16_t address) {
  if (address < 0x2000)
    return console.getMapper()->readChr(address);
  if (address < 0x3000)
    return nameTable[console.getMapper()->mirrorAddress(address) - 0x2000];
  if ((0x3f00 <= address) && (address < 0x4000)) {
    uint16_t pointer =  address % 32;
    if (pointer >= 16 && (pointer % 4) == 0)
      pointer -= 16;
    return palette[pointer];
  }
  else {
    log.error() << "UNEXPECTED READ AT " << hex(address) << "\n";
    return 0;
  }
}

void PPUMemory::write(uint16_t address, uint8_t value) {
  if (address < 0x2000)
    console.getMapper()->writeChr(address, value);
  else if (address < 0x3000) {
    uint16_t add = console.getMapper()->mirrorAddress(address) - 0x2000; 
    nameTable[add] = value;
  }
  else if ((0x3f00 <= address) && (address < 0x4000)) {
    uint16_t pointer =  address % 32;
    if (pointer >= 16 && (pointer % 4) == 0)
      pointer -= 16;
    palette[pointer] = value;
  }
  else {
    log.error() << "UNEXPECTED WRITE AT " << hex(address) << "\n";
  }
}
