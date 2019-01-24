#ifndef GUARD_CARTRIDGE_H
#define GUARD_CARTRIDGE_H


#include <cstdint>


class Cartridge {
    // TODO: support PRG RAM/CHR RAM
    public:
        uint8_t read_prg_rom(uint16_t pointer) { return prg_rom[pointer]; }
        void write_prg_rom(uint16_t pointer, uint8_t value) { prg_rom[pointer] = value; }
        uint8_t read_chr_rom(uint16_t pointer) { return chr_ram[pointer]; }
        void write_chr_rom(uint16_t pointer, uint8_t value) { chr_ram[pointer] = value; }
    private:
        static int PRG_UNIT_SIZE = 0x4000;
        static int CHR_UNIT_SIZE = 0x2000;
        uint8_t* prg_rom;
        uint8_t* chr_rom;
        Cartridge(int rom_unit, int ram_unit)
};

#endif
