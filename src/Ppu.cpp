#include "Ppu.h"

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
}

uint8_t PPUCTRL::read() {
    return 0;
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
}

uint8_t PPUMASK::read() {
    return 0;
}

PPUSTATUS::PPUSTATUS(PPU& _ppu): Register(_ppu) {
    sprite_overflow_flag = false;
    sprite_zero_flag = false;
    vertical_blank_started_flag = false;
}

void PPUSTATUS::write(uint8_t value) {
}

uint8_t PPUSTATUS::read() {
    return 0;
}

OAMADDR::OAMADDR(PPU& _ppu): Register(_ppu) {
    address = 0x00;
}

void OAMADDR::write(uint8_t value) {
}

uint8_t OAMADDR::read() {
    return 0;
}

OAMDATA::OAMDATA(PPU& _ppu): Register(_ppu) {
}

void OAMDATA::write(uint8_t value) {
}

uint8_t OAMDATA::read() {
    return 0;
}

PPUSCROLL::PPUSCROLL(PPU& _ppu): Register(_ppu) {
}

void PPUSCROLL::write(uint8_t value) {
}

uint8_t PPUSCROLL::read() {
    return 0;
}

PPUADDR::PPUADDR(PPU& _ppu): Register(_ppu) {
}

void PPUADDR::write(uint8_t value) {
}

uint8_t PPUADDR::read() {
    return 0;
}

PPUDATA::PPUDATA(PPU& _ppu): Register(_ppu) {
    buffered_value = 0x00;
}

void PPUDATA::write(uint8_t value) {
}

uint8_t PPUDATA::read() {
    return 0;
}

OAMDMA::OAMDMA(PPU& _ppu): Register(_ppu) {
}

void OAMDMA::write(uint8_t value) {
}

uint8_t OAMDMA::read() {
    return 0;
}

PPU::PPU(Console& _console):
    mem(_console),
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

void PPU::reset() {
    clock = 0;
    scan_line = 0;
    ppuctrl.write(0);
    ppumask.write(0);
    oamaddr.write(0);
}
    
