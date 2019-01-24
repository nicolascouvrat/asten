#include "Mapper.h"


namespace {
    Logger log = Logger::get_logger("Mapper").set_level(DEBUG);
}


Mapper *Mapper::from_nes_file(std::string file_name) {
    uint8_t raw_header[NESHeader::SIZE];
    std::vector<uint8_t> raw_data;

    std::ifstream in(file_name, std::ios::binary);
    std::istreambuf_iterator<char> it(in);
    for (int i = 0; i < NESHeader::SIZE; i++)
        raw_header[i] = *it++;
    std::copy(
        it,
        std::istreambuf_iterator<char>(),
        std::back_inserter(raw_data)
    );

    NESHeader header = parse_header(raw_header);
    log.debug() << header << "\n";
    return new NROMMapper(header, raw_data);
}

Mapper::Mapper(NESHeader h, const std::vector<uint8_t>& raw_data):
    prg_rom(new uint8_t[(h.prg_rom_size * PRG_ROM_UNIT) * sizeof(uint8_t)]),
    prg_ram(new uint8_t[h.prg_ram_size * PRG_RAM_UNIT]),
    chr_rom(new uint8_t[h.chr_rom_size * CHR_ROM_UNIT])
{
    int j = 0;
    for (int i = 0; i < h.prg_rom_size * PRG_ROM_UNIT; i++)
         prg_rom[i] = raw_data[j++];
    for (int i = 0; i < h.chr_rom_size * CHR_ROM_UNIT; i++)
        chr_rom[i] = raw_data[j++];
}


NROMMapper::NROMMapper(NESHeader h, const std::vector<uint8_t>& d):
    Mapper(h, d),
    // NROM-128 have 16kB of PRG_ROM, NROM-256 have 32kB
    is_nrom_128((h.prg_rom_size > 1) ? false : true)
{
}

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
