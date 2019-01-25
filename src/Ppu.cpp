#include "Ppu.h"
#include <string>
#include "Console.h"
#include "Cpu.h"


std::runtime_error invalid_register_op(std::string register_name, std::string op) {
    std::string err_str = "Invalid operation on register " + register_name + ": " + op;
    return std::runtime_error(err_str);
}

Register::Register(PPU& _ppu): ppu(_ppu) {}

PPUCTRL::PPUCTRL(PPU& _ppu): Register(_ppu) {
    nametable_flag = 0; // on two bits
    increment_flag = false;
    sprite_table_flag = false;
    background_table_flag = false;
    sprite_size_flag = false;
    master_slave_flag = false;
    nmi_flag = false;
}

void PPUCTRL::write(uint8_t value) {
    nametable_flag          = (value & 0b00000011) >> 0;
    increment_flag          = (value & 0b00000100) >> 2;
    sprite_table_flag       = (value & 0b00001000) >> 3;
    background_table_flag   = (value & 0b00010000) >> 4;
    sprite_size_flag        = (value & 0b00100000) >> 5;
    master_slave_flag       = (value & 0b01000000) >> 6;
    nmi_flag                = (value & 0b10000000) >> 7;
}

uint8_t PPUCTRL::read() {
    throw invalid_register_op("PPUCTRL", "read");
}

PPUMASK::PPUMASK(PPU& _ppu): Register(_ppu) {
    greyscale_flag = false;
    left_background_flag = false;
    left_sprites_flag = false;
    background_flag = false;
    sprites_flag = false;
    red_emphasis_flag = false;
    green_emphasis_flag = false;
    blue_emphasis_flag = false;
}

void PPUMASK::write(uint8_t value) {
    greyscale_flag          = (value & 0b00000001) >> 0;
    left_background_flag    = (value & 0b00000010) >> 1;
    left_sprites_flag       = (value & 0b00000100) >> 2;
    background_flag         = (value & 0b00001000) >> 3;
    sprites_flag            = (value & 0b00010000) >> 4;
    red_emphasis_flag       = (value & 0b00100000) >> 5;
    green_emphasis_flag     = (value & 0b01000000) >> 6;
    blue_emphasis_flag      = (value & 0b10000000) >> 7;
    
}

uint8_t PPUMASK::read() {
    throw invalid_register_op("PPUMASK", "read");
}

PPUSTATUS::PPUSTATUS(PPU& _ppu): Register(_ppu) {
    sprite_overflow_flag = false;
    sprite_zero_flag = false;
    vertical_blank_started_flag = false;
}

void PPUSTATUS::write(uint8_t value) {
    throw invalid_register_op("PPUSTATUS", "write");
}

uint8_t PPUSTATUS::read() {
    uint8_t value = 0x00;
    value |= vertical_blank_started_flag << 7;
    value |= sprite_zero_flag << 6;
    value |= sprite_overflow_flag << 5;
    if (ppu.get_nmi_occured())
        value |= 1 << 7;
    ppu.set_nmi_status(false, false);
    value |= ppu.get_latch_value() & 0x1f;

    vertical_blank_started_flag = false;
    ppu.set_write_toggle(false);
    return value;
}

OAMADDR::OAMADDR(PPU& _ppu): Register(_ppu) {
    address = 0x00;
}

void OAMADDR::write(uint8_t value) {
    address = address;
}

uint8_t OAMADDR::read() {
    return address;
}

OAMDATA::OAMDATA(PPU& _ppu): Register(_ppu) {}

void OAMDATA::write(uint8_t value) {
    data[ppu.get_oam_address()] = value;
    ppu.increment_oam_address();
}

uint8_t OAMDATA::read() { return data[ppu.get_oam_address()]; }

void OAMDATA::upload(const std::vector<uint8_t>& page) {
    if (page.size() != 256)
        throw invalid_register_op("OAMDATA", "upload_page");
    std::copy(page.begin(), page.end(), data);
}

PPUSCROLL::PPUSCROLL(PPU& _ppu): Register(_ppu) {}

void PPUSCROLL::write(uint8_t value) {
    if (ppu.get_write_toggle()) {
        ppu.set_temporary_vram(
            (ppu.get_temporary_vram() & 0x0c1f) | ((value & 0x7) << 12) | ((value & 0xf8) << 2)
        );
        ppu.set_write_toggle(false);
    }
    else {
        ppu.set_temporary_vram(
            (ppu.get_temporary_vram() & 0x7fe0) | (value >> 3)
        );
        ppu.set_fine_scroll(value & 0x7);
        ppu.set_write_toggle(true);
    }
}

uint8_t PPUSCROLL::read() {
    throw invalid_register_op("PPUSCROLL", "read");
}

PPUADDR::PPUADDR(PPU& _ppu): Register(_ppu) {}

void PPUADDR::write(uint8_t value) {
    if (ppu.get_write_toggle()) {
        uint16_t new_vram = (ppu.get_temporary_vram() & 0x7f00) | value;
        ppu.set_temporary_vram(new_vram);
        ppu.set_current_vram(new_vram);
        ppu.set_write_toggle(false);
    }
    else {
        ppu.set_temporary_vram(
            (ppu.get_temporary_vram() & 0x00ff) | ((value & 0x3f) << 8)
        );
        ppu.set_write_toggle(true);
    }
}

uint8_t PPUADDR::read() {
    throw invalid_register_op("PPUADDR", "read");
}

PPUDATA::PPUDATA(PPU& _ppu): Register(_ppu) {
    buffered_value = 0x00;
}

void PPUDATA::write(uint8_t value) {
    uint16_t current_vram = ppu.get_current_vram();
    ppu.get_memory().write(current_vram & 0x3fff, value);
    if (ppu.get_increment_flag())
        ppu.set_current_vram(current_vram + 32);
    else
        ppu.set_current_vram(current_vram + 1);
}

uint8_t PPUDATA::read() {
    uint8_t temp = buffered_value;
    uint16_t current_vram = ppu.get_current_vram();
    buffered_value = ppu.get_memory().read(current_vram & 0x3fff);

    if (ppu.get_increment_flag())
        ppu.set_current_vram(current_vram + 32);
    else
        ppu.set_current_vram(current_vram + 1);

    if ((current_vram & 0x3f00) == 0x3f00)
        return buffered_value; // this is a palette, return directly
    return temp;
}

OAMDMA::OAMDMA(PPU& _ppu): Register(_ppu) {
}

void OAMDMA::write(uint8_t value) {
    uint16_t address_begin = value << 8;
    uint16_t address_end = address_begin | 0xFF + 1;
    ppu.upload_to_oamdata(address_begin, address_end);
    if ((ppu.get_clock() % 2) == 0)
        ppu.make_cpu_wait(513);
    else
        ppu.make_cpu_wait(514);
}

uint8_t OAMDMA::read() {
    return 0;
}

PPU::PPU(Console& _console):
    mem(_console),
    console(_console),
    ppuctrl(PPUCTRL(*this)),
    ppumask(PPUMASK(*this)),
    ppustatus(PPUSTATUS(*this)),
    oamaddr(OAMADDR(*this)),
    oamdata(OAMDATA(*this)),
    ppuscroll(PPUSCROLL(*this)),
    ppuaddr(PPUADDR(*this)),
    ppudata(PPUDATA(*this)),
    oamdma(OAMDMA(*this))
{
    latch_value = 0;
    nmi_occured = false;
    nmi_previous = false;
    nmi_delay = 0;

    current_vram = 0;
    temporary_vram = 0;
    fine_scroll = 0;
    write_toggle = false;
    
    clock = 0;
    scan_line = 0;
    is_even_screen = true;
    frame_count = 0;

    name_table_byte = 0;
    attribute_table_byte = 0;
    lower_tile_byte = 0;
    higher_tile_byte = 0;
    background_data = 0; // 64 bits

    sprite_count = 0;

}

/* PUBLIC FUNCTIONS */
void PPU::reset() {
    clock = 0;
    scan_line = 0;
    ppuctrl.write(0);
    ppumask.write(0);
    oamaddr.write(0);
}

bool PPU::get_nmi_occured() { return nmi_occured; }

int PPU::get_latch_value() { return latch_value; }

void PPU::set_nmi_status(bool occured, bool previous) {
    nmi_occured = occured;
    nmi_previous = previous;
}

void PPU::set_write_toggle(bool status) { write_toggle = status; }

bool PPU::get_write_toggle() { return write_toggle; }

uint8_t PPU::get_oam_address() { return oamaddr.read(); }

void PPU::increment_oam_address() { oamaddr.write(oamaddr.read() + 1); }

void PPU::set_current_vram(uint16_t val){ current_vram = val; }

uint16_t PPU::get_current_vram() { return current_vram; }

void PPU::set_temporary_vram(uint16_t val){ temporary_vram = val; }

uint16_t PPU::get_temporary_vram() { return temporary_vram; }

void PPU::set_fine_scroll(uint8_t val){ fine_scroll = val; }

bool PPU::get_increment_flag() { return ppuctrl.increment_flag; }

PPUMemory& PPU::get_memory() { return mem; }

void PPU::upload_to_oamdata(uint16_t begin, uint16_t end) {
    std::vector<uint8_t> buffer(end - begin);
    for (int i = 0; i < end - begin; i++)
        buffer[i] = console.get_cpu().get_memory().read(begin + i);
    oamdata.upload(buffer); 
}

void PPU::make_cpu_wait(int cycles) {
    console.get_cpu().wait_for(cycles);
}

long PPU::get_clock() { return clock; }
    
/* PRIVATE FUNCTIONS */
void PPU::nmi_change() {
    bool nmi = ppuctrl.nmi_flag && nmi_occured;
    if (nmi && !nmi_previous)
        nmi_delay = 15;
    nmi_previous = nmi;
}


