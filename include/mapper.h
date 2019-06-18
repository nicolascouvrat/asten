#ifndef GUARD_MAPPER_H
#define GUARD_MAPPER_H


#include <cstdint>
#include <string>
#include <fstream>
#include <iterator>
#include <vector>
#include <iomanip>
#include <iostream>

#include "logger.h"
#include "utilities.h"


struct NESHeader {
  // in units (see Mapper for unit size)
  static const int SIZE = 0x10;
  int chrRomSize, prgRomSize, prgRamSize;
  int mapperId, mirrorId;
};

inline NESHeader parseHeader(uint8_t *rawHeader) {
  // TODO: support more options (only 255 mappers and 2 mirrors for now)
  return NESHeader {
    rawHeader[5], rawHeader[4], rawHeader[8],
    rawHeader[6] >> 4, rawHeader[6] & 1
  };
}

inline std::ostream& operator<< (std::ostream& out, const NESHeader& h) {
  return out
    << std::left
    << "PRG_ROM: " << std::setw(3) << h.prgRomSize
    << "PRG_RAM: " << std::setw(3) << h.prgRamSize 
    << "CHR_ROM: " << std::setw(3) << h.chrRomSize
    << "MIRROR: " << std::setw(3) << h.mirrorId
    << "MAPPER: " << std::setw(3) << h.mapperId;
}

class PPUMirror {
  public:
    virtual int getTable(int) = 0;
    static PPUMirror* fromId(int);
    static const int OFFSET = 0x2000;
    static const int TABLE_SIZE = 0x400;
};

class VerticalMirror: public PPUMirror {
  public:
    int getTable(int);

};

class NoMirror: public PPUMirror {
  public:
    int getTable(int);
};

class HorizontalMirror: public PPUMirror {
  public:
    int getTable(int);
};

// Mapper emulates the combination of a NES cartridge and its circuits
class Mapper {
  public:
    virtual uint8_t readPrg(uint16_t) = 0;
    virtual void writePrg(uint16_t, uint8_t) = 0;
    virtual uint8_t readChr(uint16_t) = 0;
    virtual void writeChr(uint16_t, uint8_t) = 0;
    // Call to signify that PPU A12 had a rising edge
    virtual void clockIRQCounter() = 0;
    static Mapper *fromNesFile(std::string fileName);
    // mirrorAddress is used to get the right nametable depending on the
    // mirroring
    uint16_t mirrorAddress(uint16_t);
  protected:
    Logger log;
    PPUMirror* mirror;
    Mapper(NESHeader, const std::vector<uint8_t>&);
    static const int PRG_ROM_UNIT = 0x4000;
    static const int CHR_ROM_UNIT = 0x2000;
    static const int PRG_RAM_UNIT = 0x2000;
    uint8_t *prgRom;
    uint8_t *prgRam;
    uint8_t *chrRom;
    int prgRomSize;
};

class NROMMapper: public Mapper {
  public:
    uint8_t readPrg(uint16_t p);
    void writePrg(uint16_t p, uint8_t v);
    uint8_t readChr(uint16_t p);
    void writeChr(uint16_t p, uint8_t v);
    // Does nothing
    void clockIRQCounter();
    NROMMapper(NESHeader, const std::vector<uint8_t>&);
  private:
    const bool isNrom_128;
};

// MMC3Mapper has a total of 8 banks, but controls a total of 
class MMC3Mapper: public Mapper {
  public:
    uint8_t readPrg(uint16_t p);
    void writePrg(uint16_t p, uint8_t v);
    uint8_t readChr(uint16_t p);
    void writeChr(uint16_t p, uint8_t v);
    MMC3Mapper(NESHeader, const std::vector<uint8_t>&);
    // this should be called on each rise of PPU A12, and will decrement the
    // counter and/or perform other operations (reloads...) depending on the
    // mapper's internal registers
    //
    // If the counter reaches 0 and IRQ are not disabled, this will generate and
    // IRQ interrupt
    void clockIRQCounter();
  private:
    // the size of one memory page (8kb)
    static const int PAGE_SIZE = 0x2000;
    // index of the bank to update
    uint8_t currentBank;
    // mapper of one bank to its index (i.e. which page of memory should it
    // point to)
    uint8_t bankIndexes[8];

    // although the MMC3 only has two banks dedicated to the CPU, it in fact
    // controls the offsets for 4 pages at all time (0x8000 to 0xffff)
    int cpuOffsets[4];

    // false: 0x8000 - 0x9fff swappable, 0xc000 - 0xdfff fixed to second to last
    // true: 0xc000 - 0xdfff swappable, 0x8000 - 0x9fff fixed to second to last
    bool prgROMMode;
    bool chrInversion;

    // IRQ counter
    // The counter supports values up from 1 to 256. It is not possible to
    // directly interact with it, only to indirectly "control" it through writes
    // to adresses >= 0xc000
    uint8_t IRQCounter;
    // The irq latch stores the value that will be loaded in the counter on next
    // reload
    uint8_t IRQLatch;
    // IRQ reload is not instantaneous and will happen at the next counter clock
    bool IRQReload;
    // flag that denotes if IRQ interrupts are enabled. Note that having it
    // disabled does not prevent the counter from decrementing, only the
    // interrupts from happening
    bool IRQEnabled;

    void writeBankSelect(uint8_t);
    void writeBankData(uint8_t);
    void writeMirroring(uint8_t);
    void writePRGRAMProtect(uint8_t);
    void writeIRQLatch(uint8_t);
    void writeIRQReload(uint8_t);
    void writeIRQDisable(uint8_t);
    void writeIRQEnable(uint8_t);

    void setCpuOffsets();
    int computeOffset(int);
};

#endif
