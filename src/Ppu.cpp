#include "Ppu.h"
#include <string>
#include "Console.h"
#include "Cpu.h"
#include <iostream>


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
    if (ppu.get_nmi_occured()) {
        value |= 1 << 7;
    }
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

uint8_t OAMDATA::read_index(uint8_t index) { return data[index]; }

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
    ppu.get_memory().write(ppu.current_vram & 0x3fff, value);
    // uint16_t current_vram = ppu.get_current_vram();
    // std::cout << hex(current_vram) << " " << hex(ppu.current_vram) << "\n";
    // uint16_t add = current_vram & 0x3fff;
    // PPUMemory& mem = ppu.get_memory();
    // mem.write(add, value);
    if (ppu.get_increment_flag())
        ppu.current_vram += 32;
        // ppu.set_current_vram(current_vram + 32);
    else {
        ppu.current_vram += 1;
        // ppu.set_current_vram(current_vram + 1);
    }
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
    throw invalid_register_op("OAMDMA", "read");
}

PPUStateData PPU::dump_state() {
    PPUStateData d;
    d.clock = clock;
    d.frame_count = frame_count;
    d.scan_line = scan_line;
    d.latch_value = latch_value;
    return d;
}

PPU::PPU(Console& _console):
    mem(_console),
    console(_console),
    log(Logger::get_logger("PPU", "ppu.log")),
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
    // log.set_level(DEBUG);

}

/* PUBLIC FUNCTIONS */
void PPU::reset() {
    clock = 340;
    scan_line = 240;
    ppuctrl.write(0);
    ppumask.write(0);
    oamaddr.write(0);
}

bool PPU::get_nmi_occured() { 
    return nmi_occured;
}

int PPU::get_latch_value() { 
    return latch_value; 
}

void PPU::set_nmi_status(bool occured, bool previous) {
    nmi_occured = occured;
    nmi_previous = previous;
}

void PPU::set_write_toggle(bool status) { write_toggle = status; }

bool PPU::get_write_toggle() { return write_toggle; }

uint8_t PPU::get_oam_address() { return oamaddr.read(); }

void PPU::increment_oam_address() { oamaddr.write(oamaddr.read() + 1); }

void PPU::set_current_vram(uint16_t val){ current_vram = val; }

uint16_t PPU::get_current_vram() { 
    return current_vram;
}

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
    
uint8_t PPU::read_register(uint16_t address) {
    uint8_t value;
    switch(address) {
        case 0x2000:
            return ppuctrl.read();
        case 0x2001:
            return ppumask.read();
        case 0x2002:
            value = ppustatus.read();
            return value;
        case 0x2003:
            return oamaddr.read();
        case 0x2004:
            return oamdata.read();
        case 0x2005:
            return ppuscroll.read();
        case 0x2006:
            return ppuaddr.read();
        case 0x2007:
            return ppudata.read();
        case 0x4014:
            return oamdma.read();
        default:
            throw std::runtime_error("Invalid PPU read address=" + address);
    }
}

void PPU::write_register(uint16_t address, uint8_t value) {
    latch_value = value;
    switch(address) {
        case 0x2000:
             ppuctrl.write(value);
             break;
        case 0x2001:
             ppumask.write(value);
             break;
        case 0x2002:
             ppustatus.write(value);
             break;
        case 0x2003:
             oamaddr.write(value);
             break;
        case 0x2004:
             oamdata.write(value);
             break;
        case 0x2005:
             ppuscroll.write(value);
             break;
        case 0x2006:
             ppuaddr.write(value);
             break;
        case 0x2007:
             ppudata.write(value);
             break;
        case 0x4014:
             oamdma.write(value);
             break;
        default:
            throw std::runtime_error("Invalid PPU write address=" + address);
    }
}

/* PRIVATE FUNCTIONS */
void PPU::nmi_change() {
    bool nmi = ppuctrl.nmi_flag && nmi_occured;
    if (nmi && !nmi_previous)
        nmi_delay = 15;
    nmi_previous = nmi;
}

void PPU::next_screen() {
  is_even_screen = !is_even_screen;
  frame_count++;
  console.get_engine().render();
}

void PPU::tick() {
    if (nmi_delay > 0) {
        nmi_delay--;
        if (nmi_delay == 0 && ppuctrl.nmi_flag && nmi_occured)
            console.get_cpu().trigger_nmi();
    }
    if ((ppumask.sprites_flag || ppumask.background_flag)
        && !is_even_screen && scan_line == 261 && clock == 339) {
        clock = 0;
        scan_line = 0;
        next_screen();
        return;
    }

    clock++;
    if (clock == PPU::CLOCK_CYCLE) {
        clock = 0;
        scan_line++;
        if (scan_line > PPU::PRE_RENDER_SCAN_LINE) {
            scan_line = 0;
            next_screen();
        }
    }
}
void PPU::clear_vertical_blank() {
    nmi_occured = false;
    nmi_change();
}

void PPU::set_vertical_blank() {
    nmi_occured = true;
    nmi_change();
}

void PPU::increment_horizontal_scroll() {
    if ((current_vram & 0x1f) == 31) {
        // set coarse x to 0, switch horizontal nametable
        current_vram &= 0xffe0;
        current_vram ^= 0x400;
    }
    else current_vram += 1; // increment coarse x
}

void PPU::increment_vertical_scroll() {
    if ((current_vram & 0x7000) != 0x7000)
        current_vram += 0x1000;
    else {
        current_vram &= 0x8fff;
        int coarse_y = (current_vram & 0x3e0) >> 5;
        if (coarse_y == 29) {
            coarse_y = 0;
            current_vram ^= 0x800;
        }
        else if (coarse_y == 31)
            coarse_y = 0;
        else
            coarse_y++;
        current_vram = (current_vram & 0xfc1f) | (coarse_y << 5);
    }
}

void PPU::copy_horizontal_scroll() {
    current_vram = (current_vram & 0xfbe0) | (temporary_vram & 0x041f);
}

void PPU::copy_vertical_scroll() {
    current_vram = (current_vram & 0x841f) | (temporary_vram & 0x7be0);
}

int PPU::fetch_sprite_graphics(int num, int row) {
    /* Fetches the pixel data for sprite num, 
     * where row is the row WITHIN the sprite (0 = top) 
     * */
    uint8_t tile_index = oamdata.read_index(num * 4 + 1);
    uint8_t attributes = oamdata.read_index(num * 4 + 2);
    bool vertical_flip = (attributes & 0x80) == 0x80;
    bool horizontal_flip = (attributes & 0x40) == 0x40;
    bool table;
    if (!ppuctrl.sprite_size_flag) {
        if (vertical_flip) row = 7 - row;
        table = ppuctrl.sprite_table_flag;
    }
    else {
        if (vertical_flip) row = 15 - row;
        table = tile_index & 1;
        tile_index &= 0xfe;
        if (row > 7) {
            tile_index++;
            row -= 8;
        }
    }
    uint16_t address = 0x1000 * table + 0x10 * tile_index + row;
    uint8_t low_tile_byte = mem.read(address);
    uint8_t high_tile_byte = mem.read(address + 8);
    // combine the data for 8 pixels
    uint8_t a, b, c;
    int _sprite_graphics = 0;
    a = (attributes & 0b11) << 2;
    for (int i = 0; i < 8; i++) {
        if (horizontal_flip) {
            b = (high_tile_byte & 1) << 1;
            c = (high_tile_byte & 1) << 0;
            low_tile_byte >>= 1;
            high_tile_byte >>= 1;
        }
        else {
            b = (high_tile_byte & 0x80) >> 6;
            c = (low_tile_byte & 0x80) >> 7;
            low_tile_byte <<= 1;
            high_tile_byte <<= 1;
        }
        _sprite_graphics <<= 4;
        _sprite_graphics |= (a | b | c);
    }
    return _sprite_graphics;
}

void PPU::load_sprite_data() {
    // gets all sprite data for current scan line
    int height = ppuctrl.sprite_size_flag ? 16 : 8;
    int _sprite_count = 0;
    int x, y, attribute_data, top, bottom;
    for (int i = 0; i < 64; i++) {
        x = oamdata.read_index(i * 4 + 3);
        attribute_data = oamdata.read_index(i * 4 + 2);
        y = oamdata.read_index(i * 4);
        top = y;
        bottom = y + height;
        if ((scan_line < top) || (scan_line > bottom)) continue;
        _sprite_count++;
        if (_sprite_count <= 8) {
            sprite_graphics[_sprite_count - 1] = fetch_sprite_graphics(i, scan_line - top);
            sprite_positions[_sprite_count - 1] = x;
            sprite_priorities[_sprite_count - 1] = (attribute_data >> 5) & 1;
            sprite_indexes[_sprite_count - 1] = i;
        }
    }
    if (_sprite_count > 8) {
        // no rendering if we hit more than 8 sprites, but set overflow flag
        _sprite_count = 8;
        ppustatus.sprite_overflow_flag = true;
    }
    sprite_count = _sprite_count;
}

uint8_t PPU::get_background_pixel() {
    /* Gets pixel to render from the pre-loaded 64 bits of background data
     * The progressive shift is done in the main loop.
     * */
    if (!ppumask.background_flag) return 0;
    uint32_t cycle_data = background_data >> 32;
    log.debug() << hex(background_data) << "cycle: " << hex(cycle_data) << "\n";
    uint8_t pixel_data = (cycle_data >> (7 - fine_scroll) * 4) & 0xf; 
    return pixel_data;
}

void PPU::load_background_data() {
    /* Due to the fact that get_background_pixel() consumes data,
     * we need to refill it (or rather pre fill it in the previous cycle). 
     * One pixel needs 4 bits of info, total of 32 bits/cycle.
     * */
    uint32_t data = 0;
    uint8_t a, b, c;
    // attribute_table_byte in fact contains the information twice (as there are
    // only 4 palettes, hence 4 bytes needed). Trim and pass to higher bits.
    a = (attribute_table_byte & 0b11) << 2;
    log.debug() << hex(higher_tile_byte) << hex(lower_tile_byte) << " attr:" << hex(attribute_table_byte) << "\n";
    for (int i = 0; i < 8; i ++) {
        b = (higher_tile_byte & 0x80) >> 6;
        c = (lower_tile_byte & 0x80) >> 7;
        data <<= 4;
        higher_tile_byte <<= 1;
        lower_tile_byte <<= 1;
        data |= (a | b | c);
    }
    log.debug() << "old_back " << hex(background_data) << " data " << hex(data) << "\n";
    background_data |= data;
}

void PPU::fetch_nametable_byte() {
    /* Given by 12 lowests bits of VRAM + $2000 offset */
    name_table_byte = mem.read(0x2000 + (current_vram & 0xfff));
}

void PPU::fetch_attribute_table_byte() {
    /* To get the attribute table byte, we need to combine:
     *     - the 2 bits selecting the name table,
     *     - the three highest bits of the coarse Y scroll,
     *     - the three highest bits of the coarse X scroll
     * With a #23C0 offset.
     * */
    uint16_t address = 0x23c0;
    address |= current_vram & 0xc00;
    address |= (current_vram & 0x380) >> 4;
    address |= (current_vram & 0x1c) >> 2;
    attribute_table_byte = mem.read(address);
}

void PPU::fetch_lower_tile_byte() {
    /*
     * To fetch a tile byte, combine:
     *  - table index(stored in PPUCTRL) * $1000
     *  - tile index (in the name table byte) * $10
     *  - fine Y scroll (3 highest bits of VRAM address)
     * */
    uint8_t fine_y = (current_vram >> 12) & 0x7;
    uint8_t table_index = ppuctrl.background_table_flag;
    uint8_t tile_index = name_table_byte;
    uint16_t address = 0x1000 * table_index + 0x10 * tile_index + fine_y;
    lower_tile_byte = mem.read(address);
}

void PPU::fetch_higher_tile_byte() {
    /* Read one byte above the lower tile byte */
    uint8_t fine_y = (current_vram >> 12) & 0x7;
    uint8_t table_index = ppuctrl.background_table_flag;
    uint8_t tile_index = name_table_byte;
    uint16_t address = 0x1000 * table_index + 0x10 * tile_index + fine_y;
    higher_tile_byte = mem.read(address + 8);
}

void PPU::step() {
    tick();
    bool rendering_enabled = ppumask.background_flag || ppumask.sprites_flag;
    bool is_visible_clock = (clock >= 1) && (clock <= 256);
    bool is_visible_line = scan_line < PPU::POST_RENDER_SCAN_LINE;
    bool is_prerender_line = scan_line == PPU::PRE_RENDER_SCAN_LINE;
    bool is_postrender_line = scan_line == PPU::POST_RENDER_SCAN_LINE;
    bool is_fetch_line = is_visible_line || is_prerender_line;
    bool is_fetch_clock = is_visible_clock || ((clock <= 336) && (clock >= 321));
    if (rendering_enabled) {
        if (is_visible_line && is_visible_clock) render_pixel();
        if (is_fetch_line) {
            if (is_fetch_clock) {
                int _switch = clock % 8;
                // make sure we have 8 new bits every 2 ticks
                background_data <<= 4;
                switch (_switch) {
                    case 0:
                        increment_horizontal_scroll();
                        load_background_data();
                        break;
                    case 1:
                        fetch_nametable_byte();
                        break;
                    case 3:
                        fetch_attribute_table_byte();
                        break;
                    case 5:
                        fetch_lower_tile_byte();
                        break;
                    case 7:
                        fetch_higher_tile_byte();
                        break;
                    default:
                        break;
                }

            }
            if (clock == 256) increment_vertical_scroll();

            if (clock == 257) copy_horizontal_scroll();

            if (clock == 257) load_sprite_data();
        }
        if (is_prerender_line && (clock >= 280) && (clock <= 304))
            copy_vertical_scroll();
    }
    if (is_prerender_line && (clock == 1)) {
        clear_vertical_blank();
        ppustatus.sprite_overflow_flag = false;
        ppustatus.sprite_zero_flag = false;
    }

    if (is_postrender_line && (clock == 1)) set_vertical_blank();

}

SpritePixel PPU::get_sprite_pixel() {
    if (!ppumask.sprites_flag) return {0, 0};
    for (int i = 0; i < sprite_count; i++) {
        int offset = (clock - 1) - sprite_positions[i];
        if ((offset < 0) || (offset > 7)) continue;
        uint8_t color = (sprite_graphics[i] >> (7 - offset) * 4) & 0xf;
        if (color % 4 == 0) continue; // sprite is transparent
        return {i, color};
    }
    return {0, 0};
}
void PPU::render_pixel() {
    int x = clock - 1;
    int y = scan_line;
    uint8_t background = get_background_pixel();
    int color;
    SpritePixel sprite_pix = get_sprite_pixel();
    if ((x < 8) && !ppumask.left_background_flag) background = 0;
    if ((x < 8) && !ppumask.left_sprites_flag) sprite_pix.color = 0;
    bool b_opaque = background % 4 != 0;
    bool s_opaque = sprite_pix.color % 4 != 0;
    if (!s_opaque && !b_opaque) color = 0;
    else if (!b_opaque && s_opaque) color = sprite_pix.color | 0x10;
    else if (!s_opaque && b_opaque) color = background;
    else {
        if ((sprite_indexes[sprite_pix.num] == 0) && (x < 255))
            ppustatus.sprite_zero_flag = true;
        if (sprite_priorities[sprite_pix.num] == 0)
            color = sprite_pix.color | 0x10;
        else
            color =  background;
    }
    uint8_t palette_info = mem.read(0x3f00 + color % 64);
    log.debug() << "(" << x << "," << y << ")" << ": " << hex(palette_info) << "\n";
    console.get_engine().colorPixel(x, y, palette_info);
}
