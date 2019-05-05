#include "mapper.h"

#include <cerrno>
#include <cstring>
#include <iostream>


std::runtime_error invalidNesFileError(std::string fileName) {
  return std::runtime_error(fileName + ": " + std::strerror(errno));
}

Mapper *Mapper::fromNesFile(std::string fileName) {
  uint8_t rawHeader[NESHeader::SIZE];

  std::ifstream in(fileName, std::ios::binary);
  if (!in) throw invalidNesFileError(fileName);

  auto beginning = in.tellg();
  in.seekg(0, std::ios::end);
  auto size = std::size_t(in.tellg() - beginning);
  // populate header
  in.seekg(0, std::ios::beg);
  if (size < NESHeader::SIZE) throw invalidNesFileError(fileName);

  if (!in.read((char*)rawHeader, NESHeader::SIZE))
    throw invalidNesFileError(fileName);

  // populate data
  in.seekg(NESHeader::SIZE, std::ios::beg);
  std::vector<uint8_t> rawData(size - NESHeader::SIZE);
  if (!in.read((char*)rawData.data(), rawData.size()))
    throw invalidNesFileError(fileName);

  NESHeader header = parseHeader(rawHeader);
  return new NROMMapper(header, rawData);
}

Mapper::Mapper(NESHeader header, const std::vector<uint8_t>& rawData):
  log(Logger::getLogger("Mapper")),
  mirror(PPUMirror::fromId(header.mirrorId)),
  prgRom(new uint8_t[header.prgRomSize * PRG_ROM_UNIT]),
  prgRam(new uint8_t[header.prgRamSize * PRG_RAM_UNIT]),
  chrRom(new uint8_t[header.chrRomSize * CHR_ROM_UNIT])
{
  log.setLevel(DEBUG);
  log.debug() << header << "\n";
  int j = 0;
  for (int i = 0; i < header.prgRomSize * PRG_ROM_UNIT; i++)
    prgRom[i] = rawData[j++];
  for (int i = 0; i < header.chrRomSize * CHR_ROM_UNIT; i++)
    chrRom[i] = rawData[j++];
}

uint16_t Mapper::mirrorAddress(uint16_t address) {
  int tableNumber, pointer;
  tableNumber = (address - PPUMirror::OFFSET) / PPUMirror::TABLE_SIZE;
  pointer = (address - PPUMirror::OFFSET) % PPUMirror::TABLE_SIZE;
  pointer += PPUMirror::OFFSET + mirror->getTable(tableNumber) * PPUMirror::TABLE_SIZE;
  return pointer;
}

NROMMapper::NROMMapper(NESHeader h, const std::vector<uint8_t>& d):
  Mapper(h, d),
  // NROM-128 have 16kB of PRG_ROM, NROM-256 have 32kB
  isNrom_128((h.prgRomSize > 1) ? false : true)
{}

uint8_t NROMMapper::readPrg(uint16_t address) {
  if (address < 0x8000)
    return prgRam[address - 0x6000];
  else
    return isNrom_128 ? prgRom[(address - 0x8000) % 0x4000] : prgRom[address - 0x8000];
}

void NROMMapper::writePrg(uint16_t address, uint8_t value) {
  if (address < 0x8000)
    prgRam[address - 0x6000] = value;
  else
    log.error() << "Trying to write prg at " << hex(address) << "\n";
}

uint8_t NROMMapper::readChr(uint16_t address) {
  return chrRom[address];
}

void NROMMapper::writeChr(uint16_t address, uint8_t value) {
  chrRom[address] = value;
}

PPUMirror* PPUMirror::fromId(int id) {
  if (id == 0) {
    return new HorizontalMirror();
  }
  if (id == 1) {
    return new VerticalMirror();
  }
  return new NoMirror();
}

int NoMirror::getTable(int num) {
  int mirrorPattern[4] = {1, 2, 3, 4};
  return mirrorPattern[num];
}

int VerticalMirror::getTable(int num) {
  int mirrorPattern[4] = {0, 1, 0, 1};
  return mirrorPattern[num];
}

int HorizontalMirror::getTable(int num) {
  int mirrorPattern[4] = {0, 0, 2, 2};
  return mirrorPattern[num];
}
