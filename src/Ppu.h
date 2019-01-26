#ifndef GUARD_PPU_H
#define GUARD_PPU_H

#include <cstdint>
#include <vector>
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
        friend class PPU;
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
        friend class PPU;
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
        void upload(const std::vector<uint8_t>&);
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
        const static int CLOCK_CYCLE = 341;
        const static int VISIBLE_CLOCK_CYLE = 256;
        const static int PRE_RENDER_SCAN_LINE = 261;
        const static int POST_RENDER_SCAN_LINE = 240;
        PPU(Console& console);
        uint8_t read_register(uint16_t);
        void write_register(uint16_t, uint8_t);
        void reset();
        void step();
        bool get_nmi_occured();
        void set_nmi_status(bool, bool);
        int get_latch_value();
        void set_write_toggle(bool);
        bool get_write_toggle();
        void set_current_vram(uint16_t);
        uint16_t get_current_vram();
        void set_temporary_vram(uint16_t);
        uint16_t get_temporary_vram();
        void set_fine_scroll(uint8_t);
        uint8_t get_oam_address();
        void increment_oam_address();
        bool get_increment_flag();
        PPUMemory& get_memory();
        void upload_to_oamdata(uint16_t, uint16_t);
        void make_cpu_wait(int);
        long get_clock();
    private:
        void tick();
        void nmi_change();
        void fetch_higher_tile_byte();
        void fetch_lower_tile_byte();
        void fetch_attribute_table_byte();
        void fetch_nametable_byte();
        uint8_t get_background_pixel();
        void load_background_data();
        uint8_t fetch_sprite_graphics(int, int);
        void load_sprite_data();
        void copy_horizontal_scroll();
        void copy_vertical_scroll();
        void increment_horizontal_scroll();
        void increment_vertical_scroll();
        void vertical_blank();


        PPUMemory mem;
        Console& console;
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

        uint16_t current_vram, temporary_vram;
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
