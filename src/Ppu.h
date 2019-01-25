#ifndef GUARD_PPU_H
#define GUARD_PPU_H

#include "Memory.h"

class PPU;
class Register {
    public:
        virtual uint8_t read() = 0;
        virtual void write(uint8_t) = 0;
        Register(PPU&);
    protected:
        PPU& ppu;
};

class PPUCTRL: public Register {
    public:
        uint8_t read();
        void write(uint8_t);
        PPUCTRL(PPU&);
    private:
        int nametable_flag; // on two bits
        bool increment_flag;
        bool background_table_flag, sprite_table_flag;
        bool sprite_size_flag;
        bool master_slave_flag, nmi_flag;
};

class PPUMASK: public Register {
    public:
        uint8_t read();
        void write(uint8_t);
        PPUMASK(PPU&);
    private:
        bool greyscale_flag;
        bool left_background_flag, left_sprites_flag;
        bool background_flag, sprites_flag;
        bool red_emphasis_flag, green_emphasis_flag, blue_emphasis_flag;
};

class PPUSTATUS: public Register {
    public:
        uint8_t read();
        void write(uint8_t);
        PPUSTATUS(PPU&);
    private:
        bool sprite_overflow_flag;
        bool sprite_zero_flag;
        bool vertical_blank_started_flag;
};

class OAMADDR: public Register {
    public:
        uint8_t read();
        void write(uint8_t);
        OAMADDR(PPU&);
    private:
        uint8_t address;
};

class OAMDATA: public Register {
    public:
        uint8_t read();
        void write(uint8_t);
        OAMDATA(PPU&);
    private:
        uint8_t data[256];
};

class PPUSCROLL: public Register {
    public:
        uint8_t read();
        void write(uint8_t);
        PPUSCROLL(PPU&);
};

class PPUADDR: public Register {
    public:
        uint8_t read();
        void write(uint8_t);
        PPUADDR(PPU&);
};

class PPUDATA: public Register {
    public:
        uint8_t read();
        void write(uint8_t);
        PPUDATA(PPU&);
    private:
        uint8_t buffered_value;
};

class OAMDMA: public Register {
    public:
        uint8_t read();
        void write(uint8_t);
        OAMDMA(PPU&);
};

class Console;
class PPU {
    public:
        PPU(Console& console);
        void reset();
        void step();
        friend class Register;
    private:
        PPUMemory mem;
        // TODO: check
        int latch_value;
        
        // NMI
        bool nmi_occured, nmi_previous;
        int nmi_delay;
        
        // Registers
        PPUCTRL ppuctrl;
        PPUMASK ppumask;
        PPUSTATUS ppustatus;
        OAMADDR oamaddr;
        OAMDATA oamdata;
        PPUSCROLL ppuscroll;
        PPUADDR ppuaddr;
        PPUDATA ppudata;
        OAMDMA oamdma;

        uint8_t current_vram, temporary_vram;
        uint8_t fine_scroll;
        bool write_toggle;
        
        // Rendering
        long clock, frame_count;
        int scan_line;
        bool is_even_screen;

        // Background temp vars
        uint8_t name_table_byte, attribute_table_byte;
        uint8_t lower_tile_byte, higher_tile_byte;
        int background_data; // 64 bits

        // Sprite temp vars
        int sprite_count;
        int sprite_graphics[8];
        uint8_t sprite_positions[8];
        uint8_t sprite_priorities[8];
        uint8_t sprite_indexes[8];
};
#endif
