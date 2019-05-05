#ifndef GUARD_MEMORY_H
#define GUARD_MEMORY_H

#include <iterator>
#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>

#include "utilities.h"
#include "logger.h"

class Console;

class Memory {
  public:
    void debugDump(uint16_t, uint16_t, uint16_t perLine = 0x10);
    virtual uint8_t read(uint16_t) = 0;
    virtual void write(uint16_t, uint8_t) = 0;
  protected:
    Logger log;
    Console& console;
    Memory(Console&, Logger);
};

class CPUMemory: public Memory {
  public:
    uint8_t read(uint16_t);
    void write(uint16_t, uint8_t);
    CPUMemory(Console&); 
  private:
    static const int RAM_SIZE = 0x800;
    uint8_t ram[RAM_SIZE];
};

class PPUMemory: public Memory {
  public:
    uint8_t read(uint16_t);
    void write(uint16_t, uint8_t);
    PPUMemory(Console&); 
  private:
    static const int PALETTE_SIZE = 0x0020;
    static const int NAME_TABLE_SIZE = 0x1000;
    uint8_t palette[PALETTE_SIZE];
    uint8_t nameTable[NAME_TABLE_SIZE];
};

#endif
