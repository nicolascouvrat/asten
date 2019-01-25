#include "Mapper.h"
#include <cerrno>
#include <cstring>
#include <iostream>


std::runtime_error invalid_nes_file_error(std::string file_name) {
    return std::runtime_error(file_name + ": " + std::strerror(errno));
}

Mapper *Mapper::from_nes_file(std::string file_name) {
    uint8_t raw_header[NESHeader::SIZE];

    std::ifstream in(file_name, std::ios::binary);
    if (!in) throw invalid_nes_file_error(file_name);

    auto beginning = in.tellg();
    in.seekg(0, std::ios::end);
    auto size = std::size_t(in.tellg() - beginning);
    // populate header
    in.seekg(0, std::ios::beg);
    if (size < NESHeader::SIZE) throw invalid_nes_file_error(file_name);

    if (!in.read((char*)raw_header, NESHeader::SIZE))
        throw invalid_nes_file_error(file_name);

    // populate data
    in.seekg(NESHeader::SIZE, std::ios::beg);
    std::vector<uint8_t> raw_data(size - NESHeader::SIZE);
    if (!in.read((char*)raw_data.data(), raw_data.size()))
        throw invalid_nes_file_error(file_name);

    NESHeader header = parse_header(raw_header);
    return new NROMMapper(header, raw_data);
}

Mapper::Mapper(NESHeader header, const std::vector<uint8_t>& raw_data):
    prg_rom(new uint8_t[header.prg_rom_size * PRG_ROM_UNIT]),
    prg_ram(new uint8_t[header.prg_ram_size * PRG_RAM_UNIT]),
    chr_rom(new uint8_t[header.chr_rom_size * CHR_ROM_UNIT]),
    mirror(PPUMirror::from_id(header.mirror_id)),
    log(Logger::get_logger("Mapper"))
{
    log.set_level(DEBUG);
    log.debug() << header << "\n";
    int j = 0;
    for (int i = 0; i < header.prg_rom_size * PRG_ROM_UNIT; i++)
         prg_rom[i] = raw_data[j++];
    for (int i = 0; i < header.chr_rom_size * CHR_ROM_UNIT; i++)
        chr_rom[i] = raw_data[j++];
}

uint16_t Mapper::mirror_address(uint16_t address) {
    int table_number, pointer;
    table_number = (address - PPUMirror::OFFSET) / PPUMirror::TABLE_SIZE;
    pointer = (address - PPUMirror::OFFSET) % PPUMirror::TABLE_SIZE;
    pointer += PPUMirror::OFFSET + mirror.get_table(table_number) * PPUMirror::TABLE_SIZE;
    return pointer;
}

NROMMapper::NROMMapper(NESHeader h, const std::vector<uint8_t>& d):
    Mapper(h, d),
    // NROM-128 have 16kB of PRG_ROM, NROM-256 have 32kB
    is_nrom_128((h.prg_rom_size > 1) ? false : true)
{}

uint8_t NROMMapper::read_prg(uint16_t address) {
    if (address < 0x8000)
        return prg_ram[address - 0x6000];
    else
        return is_nrom_128 ? prg_rom[(address - 0x8000) % 0x4000] : prg_rom[address - 0x8000];
}

void NROMMapper::write_prg(uint16_t address, uint8_t value) {
    if (address < 0x8000)
        prg_ram[address - 0x6000] = value;
    else
        log.error() << "Trying to write prg at " << hex(address) << "\n";
}

uint8_t NROMMapper::read_chr(uint16_t address) {
    return chr_rom[address];
}

void NROMMapper::write_chr(uint16_t address, uint8_t value) {
    chr_rom[address] = value;
}

PPUMirror PPUMirror::from_id(int id) {
    switch(id) {
        case 0:
            return HorizontalMirror();
        case 1:
            return VerticalMirror();
        default:
            return PPUMirror();
    }
}

int PPUMirror::get_table(int num) {
    return mirror_pattern[num];
}
