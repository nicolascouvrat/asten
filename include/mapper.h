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

class Mapper {
  public:
    virtual uint8_t readPrg(uint16_t) = 0;
    virtual void writePrg(uint16_t, uint8_t) = 0;
    virtual uint8_t readChr(uint16_t) = 0;
    virtual void writeChr(uint16_t, uint8_t) = 0;
    static Mapper *fromNesFile(std::string fileName);
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
};

class NROMMapper: public Mapper {
  public:
    uint8_t readPrg(uint16_t p);
    void writePrg(uint16_t p, uint8_t v);
    uint8_t readChr(uint16_t p);
    void writeChr(uint16_t p, uint8_t v);
    NROMMapper(NESHeader, const std::vector<uint8_t>&);
  private:
    const bool isNrom_128;
};


#endif
