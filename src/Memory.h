#ifndef GUARD_MEMORY_H
#define GUARD_MEMORY_H

#include <iterator>
#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>

#include "Utilities.h"
#include "Logger.h"

class Console;

class Memory {
    public:
        void debug_dump(uint16_t, uint16_t, uint16_t per_line = 0x10);
        virtual uint8_t read(uint16_t) = 0;
        virtual void write(uint16_t, uint8_t) = 0;
    protected:
        Memory(Console&, Logger);
        Logger log;
        Console& console;
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
        uint8_t name_table[NAME_TABLE_SIZE];
};

#endif
