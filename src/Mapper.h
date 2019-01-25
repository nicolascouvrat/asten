#ifndef GUARD_MAPPER_H
#define GUARD_MAPPER_H


#include <cstdint>
#include <string>
#include <fstream>
#include <iterator>
#include <vector>
#include <iomanip>
#include <iostream>
#include "Logger.h"
#include "Utilities.h"


struct NESHeader {
    // in units (see Mapper for unit size)
    static const int SIZE = 0x10;
    int chr_rom_size, prg_rom_size, prg_ram_size;
    int mapper_id, mirror_id;
};

inline NESHeader parse_header(uint8_t *raw_header) {
    // TODO: support more options (only 255 mappers and 2 mirrors for now)
    return NESHeader {
        raw_header[5], raw_header[4], raw_header[8],
        raw_header[6] >> 4, raw_header[6] & 1
    };
}

inline std::ostream& operator<< (std::ostream& out, const NESHeader& h) {
    return out
        << std::left
        << "PRG_ROM: " << std::setw(3) << h.prg_rom_size
        << "PRG_RAM: " << std::setw(3) << h.prg_ram_size 
        << "CHR_ROM: " << std::setw(3) << h.chr_rom_size
        << "MIRROR: " << std::setw(3) << h.mirror_id
        << "MAPPER: " << std::setw(3) << h.mapper_id;
}

class Mapper {
    public:
        virtual uint8_t read_prg(uint16_t) = 0;
        virtual void write_prg(uint16_t, uint8_t) = 0;
        virtual uint8_t read_chr(uint16_t) = 0;
        virtual void write_chr(uint16_t, uint8_t) = 0;
        static Mapper *from_nes_file(std::string file_name);
    protected:
        Logger log;
        Mapper(NESHeader, const std::vector<uint8_t>&);
        static const int PRG_ROM_UNIT = 0x4000;
        static const int CHR_ROM_UNIT = 0x2000;
        static const int PRG_RAM_UNIT = 0x2000;
        uint8_t *prg_rom;
        uint8_t *prg_ram;
        uint8_t *chr_rom;
};

class NROMMapper: public Mapper {
    public:
        uint8_t read_prg(uint16_t p);
        void write_prg(uint16_t p, uint8_t v);
        uint8_t read_chr(uint16_t p);
        void write_chr(uint16_t p, uint8_t v);
        NROMMapper(NESHeader, const std::vector<uint8_t>&);
    private:
        const bool is_nrom_128;
};


#endif
