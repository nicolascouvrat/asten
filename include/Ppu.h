#ifndef GUARD_PPU_H
#define GUARD_PPU_H

#include <cstdint>
#include <vector>

#include "memory.h"
#include "utilities.h"

struct SpritePixel {
  int num;
  uint8_t color;
};

class PPU;
class Register {
  public:
    virtual uint8_t read() = 0;
    virtual void write(uint8_t) = 0;
    Register(PPU&);
  protected:
    PPU& ppu;
};

class PPUCTRL: public Register {
  public:
    uint8_t read();
    void write(uint8_t);
    PPUCTRL(PPU&);
    friend class PPU;
  private:
    int nametableFlag; // on two bits
    bool incrementFlag;
    bool backgroundTableFlag, spriteTableFlag;
    bool spriteSizeFlag;
    bool masterSlaveFlag, nmiFlag;
};

class PPUMASK: public Register {
  public:
    uint8_t read();
    void write(uint8_t);
    PPUMASK(PPU&);
    friend class PPU;
  private:
    bool greyscaleFlag;
    bool leftBackgroundFlag, leftSpritesFlag;
    bool backgroundFlag, spritesFlag;
    bool redEmphasisFlag, greenEmphasisFlag, blueEmphasisFlag;
};

class PPUSTATUS: public Register {
  public:
    uint8_t read();
    void write(uint8_t);
    PPUSTATUS(PPU&);
    friend class PPU;
  private:
    bool spriteOverflowFlag;
    bool spriteZeroFlag;
    bool verticalBlankStartedFlag;
};

class OAMADDR: public Register {
  public:
    uint8_t read();
    void write(uint8_t);
    OAMADDR(PPU&);
  private:
    uint8_t address;
};

class OAMDATA: public Register {
  public:
    uint8_t read();
    void write(uint8_t);
    OAMDATA(PPU&);
    void upload(const std::vector<uint8_t>&);
    uint8_t readIndex(uint8_t);
  private:
    uint8_t data[256];
};

class PPUSCROLL: public Register {
  public:
    uint8_t read();
    void write(uint8_t);
    PPUSCROLL(PPU&);
};

class PPUADDR: public Register {
  public:
    uint8_t read();
    void write(uint8_t);
    PPUADDR(PPU&);
};

class PPUDATA: public Register {
  public:
    uint8_t read();
    void write(uint8_t);
    PPUDATA(PPU&);
  private:
    uint8_t bufferedValue;
};

class OAMDMA: public Register {
  public:
    uint8_t read();
    void write(uint8_t);
    OAMDMA(PPU&);
};

class Console;
class PPU {
  public:
    const static int CLOCK_CYCLE = 341;
    const static int VISIBLE_CLOCK_CYLE = 256;
    const static int PRE_RENDER_SCAN_LINE = 261;
    const static int POST_RENDER_SCAN_LINE = 240;
    PPU(Console& console);
    PPUStateData dumpState();
    uint8_t readRegister(uint16_t);
    void writeRegister(uint16_t, uint8_t);
    void reset();
    void step();
    bool getNmiOccured();
    void setNmiStatus(bool, bool);
    int getLatchValue();
    void setWriteToggle(bool);
    bool getWriteToggle();
    void setCurrentVram(uint16_t);
    uint16_t getCurrentVram();
    void setTemporaryVram(uint16_t);
    uint16_t getTemporaryVram();
    void setFineScroll(uint8_t);
    uint8_t getOamAddress();
    void incrementOamAddress();
    bool getIncrementFlag();
    PPUMemory& getMemory();
    void uploadToOamdata(uint16_t, uint16_t);
    void makeCpuWait(int);
    long getClock();
    friend class PPUDATA;
  private:
    void tick();
    void nmiChange();
    void fetchHigherTileByte();
    void fetchLowerTileByte();
    void fetchAttributeTableByte();
    void fetchNametableByte();
    uint8_t getBackgroundPixel();
    void loadBackgroundData();
    int fetchSpriteGraphics(int, int);
    void loadSpriteData();
    void copyHorizontalScroll();
    void copyVerticalScroll();
    void incrementHorizontalScroll();
    void incrementVerticalScroll();
    void clearVerticalBlank();
    void setVerticalBlank();
    void renderPixel();
    void nextScreen();
    SpritePixel getSpritePixel();

    Logger log;
    PPUMemory mem;
    Console& console;
    // TODO: check
    int latchValue;
    
    // NMI
    bool nmiOccured, nmiPrevious;
    int nmiDelay;
    
    // Registers
    PPUCTRL ppuctrl;
    PPUMASK ppumask;
    PPUSTATUS ppustatus;
    OAMADDR oamaddr;
    OAMDATA oamdata;
    PPUSCROLL ppuscroll;
    PPUADDR ppuaddr;
    PPUDATA ppudata;
    OAMDMA oamdma;

    uint16_t currentVram, temporaryVram;
    uint8_t fineScroll;
    bool writeToggle;
    
    // Rendering
    long clock, frameCount;
    int scanLine;
    bool isEvenScreen;

    // Background temp vars
    uint8_t nameTableByte, attributeTableByte;
    uint8_t lowerTileByte, higherTileByte;
    uint64_t backgroundData; // 64 bits

    // Sprite temp vars
    int spriteCount;
    int spriteGraphics[8];
    uint8_t spritePositions[8];
    uint8_t spritePriorities[8];
    uint8_t spriteIndexes[8];
};
#endif
