#include <cerrno>
#include <cstring>
#include <iostream>

#include "mapper.h"
#include "console.h"


std::runtime_error invalidNesFileError(std::string fileName) {
  return std::runtime_error(fileName + ": " + std::strerror(errno));
}

std::runtime_error mapperNotImplementedError(std::string what) {
  return std::runtime_error("Not implemented mapper op: " + what);
}

// fromNesFile opens a .nes file, parses its header then generates an
// appropriate mapper on top of cartridge data
Mapper *Mapper::fromNesFile(Console& c, std::string fileName) {
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
      return new NROMMapper(c, header, rawData);
    case 4:
      return new MMC3Mapper(c, header, rawData);
    default:
      throw mapperNotImplementedError("mapper id n" + std::to_string(header.mapperId));
  }
}

Mapper::Mapper(Console& c, NESHeader header, const std::vector<uint8_t>& rawData):
  log(Logger::getLogger("Mapper", "mapper.log")),
  mirror(PPUMirror::fromId(header.mirrorId)),
  console(c),
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
  prgRomSize = header.prgRomSize;
}

uint16_t Mapper::mirrorAddress(uint16_t address) {
  int tableNumber, pointer;
  tableNumber = (address - PPUMirror::OFFSET) / PPUMirror::TABLE_SIZE;
  pointer = (address - PPUMirror::OFFSET) % PPUMirror::TABLE_SIZE;
  pointer += PPUMirror::OFFSET + mirror->getTable(tableNumber) * PPUMirror::TABLE_SIZE;
  return pointer;
}

NROMMapper::NROMMapper(Console& c, NESHeader h, const std::vector<uint8_t>& d):
  Mapper(c, h, d),
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

// clockIRQCounter does nothing for the NROM Mapper
void NROMMapper::clockIRQCounter() {}

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

MMC3Mapper::MMC3Mapper(Console& c, NESHeader h, const std::vector<uint8_t>& d):
  Mapper(c, h, d)
{
  cpuOffsets[0] = computeCpuOffset(0);
  cpuOffsets[1] = computeCpuOffset(1);
  cpuOffsets[2] = computeCpuOffset(-2);
  cpuOffsets[3] = computeCpuOffset(-1);

  ppuOffsets[0] = computePpuOffset(0);
  ppuOffsets[1] = computePpuOffset(1);
  ppuOffsets[2] = computePpuOffset(2);
  ppuOffsets[3] = computePpuOffset(3);
  ppuOffsets[4] = computePpuOffset(4);
  ppuOffsets[5] = computePpuOffset(5);
  ppuOffsets[6] = computePpuOffset(6);
  ppuOffsets[7] = computePpuOffset(7);
}

// readPrg returns the byte stored in PRGROM for this address
// 
// It works by finding which page the address belongs to, then reading from the
// memory using the offsets determined by the internal state of the mapper
uint8_t MMC3Mapper::readPrg(uint16_t address){
  if (address < 0x8000) {
    log.error() << "Trying to read PRG at " <<  hex(address) << "\n";
    return 0;
  }
  int index = (address - 0x8000) / MMC3Mapper::PRG_PAGE_SIZE;
  int offset = (address - 0x8000) % MMC3Mapper::PRG_PAGE_SIZE;
  int redirectedAddress = cpuOffsets[index] + offset;
  uint8_t value = prgRom[redirectedAddress];
  log.debug() << "read " << hex(value) << " at " << hex(address) << "\n";
  log.debug() << "offset " << hex(cpuOffsets[index]) << "\n";
  log.debug() << "offset " << offset << " index " << index << "\n";

  return value;
}

uint8_t MMC3Mapper::readChr(uint16_t address){
  log.debug() << "Attempted mapper read addr=" << hex(address) << "\n";
  if (address > 0x2000) {
    log.error() << "Trying to read CHR at " << hex(address) << "\n";
    return 0;
  }
  int index = address / MMC3Mapper::CHR_PAGE_SIZE;
  int offset = address % MMC3Mapper::CHR_PAGE_SIZE;
  int redirectedAddress = ppuOffsets[index] + offset;
  return chrRom[redirectedAddress];
}

// writePrg is called for address >= 0x8000
void MMC3Mapper::writePrg(uint16_t address, uint8_t value) {
  log.debug() << "write " << hex(value) << " at " << hex(address) << "\n";
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
  if (address > 0x2000) {
    log.error() << "Trying to write CHR at " << hex(address) << "\n";
    return;
  }
  int index = address / MMC3Mapper::CHR_PAGE_SIZE;
  int offset = address % MMC3Mapper::CHR_PAGE_SIZE;
  int redirectedAddress = ppuOffsets[index] + offset;
  chrRom[redirectedAddress] = value;
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
  setCpuOffsets();
  setPpuOffsets();
}

// writeBankData will set the offset of the currentBank so that it points to the
// correct location in prgRom
void MMC3Mapper::writeBankData(uint8_t value) {
  bankIndexes[currentBank] = value;
  setCpuOffsets();
  setPpuOffsets();
}

// setCpuOffsets assigns the 4 cpu memory pages (0x8000 thru 0xffff) to
// different locations in prgRom
void MMC3Mapper::setCpuOffsets() {
  // in all cases, 0xa000 - 0xbfff has the page index given by the last bank
  // MMC3 is capped at 64 pages of prgROM, so ignore the two uper bits
  cpuOffsets[1] = computeCpuOffset(bankIndexes[7] & 63);
  // in all cases, 0xe000 - 0xffff is locked to the last page
  cpuOffsets[3] = computeCpuOffset(-1);
  if (prgROMMode) {
    // 0x8000-0x9fff locked, 0xc000 - 0xdfff swappable
    cpuOffsets[0] = computeCpuOffset(-2);
    cpuOffsets[2] = computeCpuOffset(bankIndexes[6] & 63);
  } else {
    // 0x8000-0x9fff swappable, 0xc000 - 0xdfff locked
    cpuOffsets[0] = computeCpuOffset(bankIndexes[6] & 63);
    cpuOffsets[2] = computeCpuOffset(-2);
  }
}

// setPpuOffsets assigns the 8 ppu memory pages (0x0000 thru 0x1fff) to
// different locations in chrRom
void MMC3Mapper::setPpuOffsets() {
  // 2kb pages cannot select uneven banks, so ignore the lowest bit
  int r0 = computePpuOffset(bankIndexes[0] & 0xfe);
  int r1 = computePpuOffset(bankIndexes[1] & 0xfe);
  int r2 = computePpuOffset(bankIndexes[2]);
  int r3 = computePpuOffset(bankIndexes[3]);
  int r4 = computePpuOffset(bankIndexes[4]);
  int r5 = computePpuOffset(bankIndexes[5]);
  if (chrInversion) {
    // then we have 2 * 2kb banks at 0x0000 - 0x0fff and 1kb after
    ppuOffsets[0] = r0;
    ppuOffsets[1] = r0;
    ppuOffsets[2] = r1;
    ppuOffsets[3] = r1;
    ppuOffsets[4] = r2;
    ppuOffsets[5] = r3;
    ppuOffsets[6] = r4;
    ppuOffsets[7] = r5;
  } else {
    // then we have 1kb banks and 2 * 2kb at 0x1000 - 0x1fff
    ppuOffsets[0] = r2;
    ppuOffsets[1] = r3;
    ppuOffsets[2] = r4;
    ppuOffsets[3] = r5;
    ppuOffsets[4] = r0;
    ppuOffsets[5] = r0;
    ppuOffsets[6] = r1;
    ppuOffsets[7] = r1;
  }
}

// computeCpuOffset returns the offset of in memory for a given index (that can be
// negative, in which case it will read from the end)
int MMC3Mapper::computeCpuOffset(int pageIndex) {
  if (pageIndex < 0) {
    // prgRomSize is in units of 0x4000 while a mapper page is 0x2000
    pageIndex += prgRomSize * 2;
  }
  return MMC3Mapper::PRG_PAGE_SIZE * pageIndex;
}

// computePpuOffset returns the offset in memory of a given index (that has to
// be positive), depending of if the page size is 2kb or 1kb
int MMC3Mapper::computePpuOffset(int pageIndex) {
  if (pageIndex < 0) {
    throw std::runtime_error("ppu offset should not be negative");
  }
  int pageSize = MMC3Mapper::CHR_PAGE_SIZE;
  return pageSize * pageIndex;
}

void MMC3Mapper::writeMirroring(uint8_t value) {
  isHorizontalMirroring = value & 1;
}

void MMC3Mapper::writePRGRAMProtect(uint8_t value) {
  log.warn() << "writePRGRAMProtect is ignored\n";
}

// clockIRQCounter checks the counter value
//  - if it is 0 and interrupts are enabled, then a CPU IRQ is triggered
//  - if it is 0 OR if the reload flag is true, it sets the counter to the latch
//  value. Else, it decrements
//  XXX there are slight variants for this depending on the hardware, see wiki
//  https://wiki.nesdev.com/w/index.php/MMC3#IRQ_Specifics
void MMC3Mapper::clockIRQCounter() {
  if (IRQCounter == 0 && IRQEnabled) {
    console.getCpu().triggerIrq();
  }
  if (IRQCounter == 0 || IRQReload) {
    IRQReload = false;
    IRQCounter = IRQLatch;
  } else {
    IRQCounter--;
  }
}

void MMC3Mapper::writeIRQLatch(uint8_t value) {
  IRQLatch = value;
}

// writeIRQReload will trigger a reload at the next counter clock
void MMC3Mapper::writeIRQReload(uint8_t value) {
  IRQReload = true;
}

void MMC3Mapper::writeIRQDisable(uint8_t value) {
  IRQEnabled = false;
}

void MMC3Mapper::writeIRQEnable(uint8_t value) {
  IRQEnabled = true;
}
