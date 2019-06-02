#include "mapper.h"

#include <cerrno>
#include <cstring>
#include <iostream>


std::runtime_error invalidNesFileError(std::string fileName) {
  return std::runtime_error(fileName + ": " + std::strerror(errno));
}

std::runtime_error mapperNotImplementedError(std::string what) {
  return std::runtime_error("Not implemented mapper op: " + what);
}

// fromNesFile opens a .nes file, parses its header then generates an
// appropriate mapper on top of cartridge data
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

  // parse header object and return adequate mapper
  NESHeader header = parseHeader(rawHeader);
  switch(header.mapperId){
    case 0:
      return new NROMMapper(header, rawData);
    case 4:
      return new MMC3Mapper(header, rawData);
    default:
      throw mapperNotImplementedError("mapper id n" + std::to_string(header.mapperId));
  }
}

Mapper::Mapper(NESHeader header, const std::vector<uint8_t>& rawData):
  log(Logger::getLogger("Mapper")),
  mirror(PPUMirror::fromId(header.mirrorId)),
  prgRom(new uint8_t[header.prgRomSize * PRG_ROM_UNIT]),
  prgRam(new uint8_t[header.prgRamSize * PRG_RAM_UNIT]),
  chrRom(new uint8_t[header.chrRomSize * CHR_ROM_UNIT])
{
  log.setLevel(INFO);
  log.info() << header << "\n";
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

MMC3Mapper::MMC3Mapper(NESHeader h, const std::vector<uint8_t>& d):
  Mapper(h, d)
{}

uint8_t MMC3Mapper::readPrg(uint16_t address){
  log.debug() << "Attempted mapper read addr=" << hex(address) << "\n";
  return 0;
}

uint8_t MMC3Mapper::readChr(uint16_t address){
  log.debug() << "Attempted mapper read addr=" << hex(address) << "\n";
  return 0;
}

// writePrg is called for address >= 0x8000
void MMC3Mapper::writePrg(uint16_t address, uint8_t value) {
  if (address < 0x8000) {
    log.error() << "Trying to write PRG at " <<  hex(address) << "\n";
  }
  else if (address < 0xa000 && address % 2 == 0) {
    writeBankSelect(value);
  }
  else if (address < 0xa000 && address % 2 == 1) {
    writeBankData(value);
  }
  else if (address < 0xc000 && address % 2 == 0) {
    writeMirroring(value);
  }
  else if (address < 0xc000 && address % 2 == 1) {
    writePRGRAMProtect(value);
  }
  else if (address < 0xe000 && address % 2 == 0) {
    writeIRQLatch(value);
  }
  else if (address < 0xe000 && address % 2 == 1) {
    writeIRQReload(value);
  }
  else if (address % 2 == 0) {
    writeIRQDisable(value);
  }
  else {
    writeIRQEnable(value);
  }
}

void MMC3Mapper::writeChr(uint16_t address, uint8_t value) {
  log.debug() << "Attempted mapper write addr=" << hex(address) << " val=" << hex(value) << "\n";
}

// writeBankSelect sets internal MMC3 values according to value
// bits 0 - 2: bank register to update
// bits 3 - 5: nothing
// bit 6: PRG ROM mode
// bit 7: CHR A12 inversion
void MMC3Mapper::writeBankSelect(uint8_t value) {
  currentBank = value & 0x7;
  prgROMMode = (value >> 6) & 0x1;
  chrInversion = (value >> 7);
}

// writeBankData will set the offset of the currentBank so that it points to the
// correct location in prgRom
void MMC3Mapper::writeBankData(uint8_t value) {
  bankIndexes[currentBank] = value;
}

void MMC3Mapper::writeMirroring(uint8_t value) {
  throw mapperNotImplementedError("writeMirroring");
}

void MMC3Mapper::writePRGRAMProtect(uint8_t value) {
  throw mapperNotImplementedError("writePRGRAMProtect");
}

void MMC3Mapper::writeIRQLatch(uint8_t value) {
  throw mapperNotImplementedError("writeIRQLatch");
}

void MMC3Mapper::writeIRQReload(uint8_t value) {
  throw mapperNotImplementedError("writeIRQReload");
}

void MMC3Mapper::writeIRQDisable(uint8_t value) {
  throw mapperNotImplementedError("writeIRQDisable");
}

void MMC3Mapper::writeIRQEnable(uint8_t value) {
  throw mapperNotImplementedError("writeIRQEnable");
}
