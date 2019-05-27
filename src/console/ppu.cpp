#include "ppu.h"

#include <string>
#include <iostream>

#include "console.h"
#include "cpu.h"


std::runtime_error invalidRegisterOp(std::string registerName, std::string op) {
  std::string errStr = "Invalid operation on register " + registerName + ": " + op;
  return std::runtime_error(errStr);
}

Register::Register(PPU& _ppu): ppu(_ppu) {}

PPUCTRL::PPUCTRL(PPU& _ppu): Register(_ppu) {
  // Base nametable address (0 = $2000, 1: $2400, 2: $2800, 3: $2C00)
  nametableFlag = 0; // on two bits
  // VRAM address increment per read/write of PPUDATA (0: add 1, 1: add 32)
  incrementFlag = false;
  // sprite pattern table for 8x8 sprites (ignored in 8x16) (0: $0000, 1: $1000)
  spriteTableFlag = false;
  // background pattern table (0: $0000, 1: $1000)
  backgroundTableFlag = false;
  // 0: 8x8, 1: 8x16
  spriteSizeFlag = false;
  // 0: read from EXT pins, 1: output
  masterSlaveFlag = false;
  // generate a nmi at the start of vertical blanking
  nmiFlag = false;
}

// write the byte to the register. Bits are (lowest to highest):
// - base nametable address (2 bits)
// - increment flag
// - sprite pattern address
// - background pattern address
// - sprite size
// - master/slave select
// - nmi flag
void PPUCTRL::write(uint8_t value) {
  nametableFlag         = (value & 0b00000011) >> 0;
  incrementFlag         = (value & 0b00000100) >> 2;
  spriteTableFlag       = (value & 0b00001000) >> 3;
  backgroundTableFlag   = (value & 0b00010000) >> 4;
  spriteSizeFlag        = (value & 0b00100000) >> 5;
  masterSlaveFlag       = (value & 0b01000000) >> 6;
  nmiFlag               = (value & 0b10000000) >> 7;

  // t: ...BA.. ........ = d: ......BA
  ppu.setTemporaryVram (
    (ppu.getTemporaryVram() & 0xfcff) | ((value & 0x3) << 10)
  );
}

uint8_t PPUCTRL::read() {
  throw invalidRegisterOp("PPUCTRL", "read");
}

PPUMASK::PPUMASK(PPU& _ppu): Register(_ppu) {
  // 0: normal color, 1: greyscale
  greyscaleFlag = false;
  // 1: show background in leftmost 8 pixels, 0: hide
  leftBackgroundFlag = false;
  // 1: show sprites in leftmost 8 pixels, 0: hide
  leftSpritesFlag = false;
  // show background
  backgroundFlag = false;
  // show sprites
  spritesFlag = false;
  // emphasize colors
  redEmphasisFlag = false;
  greenEmphasisFlag = false;
  blueEmphasisFlag = false;
}

void PPUMASK::write(uint8_t value) {
  greyscaleFlag         = (value & 0b00000001) >> 0;
  leftBackgroundFlag    = (value & 0b00000010) >> 1;
  leftSpritesFlag       = (value & 0b00000100) >> 2;
  backgroundFlag        = (value & 0b00001000) >> 3;
  spritesFlag           = (value & 0b00010000) >> 4;
  redEmphasisFlag       = (value & 0b00100000) >> 5;
  greenEmphasisFlag     = (value & 0b01000000) >> 6;
  blueEmphasisFlag      = (value & 0b10000000) >> 7;
    
}

uint8_t PPUMASK::read() {
  throw invalidRegisterOp("PPUMASK", "read");
}

PPUSTATUS::PPUSTATUS(PPU& _ppu): Register(_ppu) {
  // supposed to be set to 1 when more than 8 sprites appear on a scan line but
  // has known hardware bug
  spriteOverflowFlag = false;
  // set when non zero pixel of sprite zero overlaps with non zero pixel of
  // background
  spriteZeroFlag = false;
  // 0: not in vblank, 1: in vblank
  verticalBlankStartedFlag = false;
}

void PPUSTATUS::write(uint8_t value) {
  throw invalidRegisterOp("PPUSTATUS", "write");
}

// read returns the following bits:
// - 5 lowest bits of whatever was previously written to the PPU (the latch
// value)
// - sprite overflow flag
// - sprite zero flag
// - vblank flag
uint8_t PPUSTATUS::read() {
  uint8_t value = ppu.getLatchValue() & 0x1f;
  value |= verticalBlankStartedFlag << 7;
  value |= spriteZeroFlag << 6;
  value |= spriteOverflowFlag << 5;
  if (ppu.getNmiOccured()) {
    value |= 1 << 7;
  }
  ppu.setNmiStatus(false, false);

  verticalBlankStartedFlag = false;
  // w:                  = 0
  ppu.setWriteToggle(false);
  return value;
}

OAMADDR::OAMADDR(PPU& _ppu): Register(_ppu) {
  // OAM address to access
  address = 0x0;
}

void OAMADDR::write(uint8_t value) {
  address = value;
}

uint8_t OAMADDR::read() {
  throw invalidRegisterOp("OAMADDR", "read");
}

OAMDATA::OAMDATA(PPU& _ppu): Register(_ppu) {}

void OAMDATA::write(uint8_t value) {
  data[ppu.getOamAddress()] = value;
  ppu.incrementOamAddress();
}

uint8_t OAMDATA::read() { return data[ppu.getOamAddress()]; }

void OAMDATA::upload(const std::vector<uint8_t>& page) {
  if (page.size() != 256)
    throw invalidRegisterOp("OAMDATA", "uploadPage");
  std::copy(page.begin(), page.end(), data);
}

PPUSCROLL::PPUSCROLL(PPU& _ppu): Register(_ppu) {}

void PPUSCROLL::write(uint8_t value) {
  if (ppu.getWriteToggle()) {
    // t: CBA..HG FED..... = d: HGFEDCBA
    // w:                  = 0
    ppu.setTemporaryVram(
      (ppu.getTemporaryVram() & 0x0c1f) | ((value & 0x7) << 12) | ((value & 0xf8) << 2)
    );
    ppu.setWriteToggle(false);
  }
  else {
    // t: ....... ...HGFED = d: HGFED...
    // x:              CBA = d: .....CBA
    // w:                  = 1
    ppu.setTemporaryVram(
      (ppu.getTemporaryVram() & 0xffe0) | (value >> 3)
    );
    ppu.setFineScroll(value & 0x7);
    ppu.setWriteToggle(true);
  }
}

uint8_t PPUSCROLL::read() {
  throw invalidRegisterOp("PPUSCROLL", "read");
}

PPUADDR::PPUADDR(PPU& _ppu): Register(_ppu) {}

void PPUADDR::write(uint8_t value) {
  if (ppu.getWriteToggle()) {
    uint16_t newVram = (ppu.getTemporaryVram() & 0x7f00) | value;
    ppu.setTemporaryVram(newVram);
    ppu.setCurrentVram(newVram);
    ppu.setWriteToggle(false);
  }
  else {
    ppu.setTemporaryVram(
      (ppu.getTemporaryVram() & 0x00ff) | ((value & 0x3f) << 8)
    );
    ppu.setWriteToggle(true);
  }
}

uint8_t PPUADDR::read() {
  throw invalidRegisterOp("PPUADDR", "read");
}

PPUDATA::PPUDATA(PPU& _ppu): Register(_ppu) {
  bufferedValue = 0x00;
}

void PPUDATA::write(uint8_t value) {
  ppu.getMemory().write(ppu.currentVram & 0x3fff, value);
  // uint16_t currentVram = ppu.getCurrentVram();
  // std::cout << hex(currentVram) << " " << hex(ppu.currentVram) << "\n";
  // uint16_t add = currentVram & 0x3fff;
  // PPUMemory& mem = ppu.getMemory();
  // mem.write(add, value);
  if (ppu.getIncrementFlag())
    ppu.currentVram += 32;
    // ppu.setCurrentVram(currentVram + 32);
  else {
    ppu.currentVram += 1;
    // ppu.setCurrentVram(currentVram + 1);
  }
}


uint8_t PPUDATA::read() {
  uint8_t temp = bufferedValue;
  uint16_t currentVram = ppu.getCurrentVram();
  bufferedValue = ppu.getMemory().read(currentVram & 0x3fff);

  if (ppu.getIncrementFlag())
    ppu.setCurrentVram(currentVram + 32);
  else
    ppu.setCurrentVram(currentVram + 1);

  if ((currentVram & 0x3f00) == 0x3f00)
    return bufferedValue; // this is a palette, return directly
  return temp;
}

OAMDMA::OAMDMA(PPU& _ppu): Register(_ppu) {
}

void OAMDMA::write(uint8_t value) {
  uint16_t addressBegin = value << 8;
  // we want to copy from 0xvalue00 to 0xvalueFF included, so add 1
  uint16_t addressEnd = (addressBegin | 0xFF) + 1;
  ppu.uploadToOamdata(addressBegin, addressEnd);
  if ((ppu.getClock() % 2) == 0)
    ppu.makeCpuWait(513);
  else
    ppu.makeCpuWait(514);
}

uint8_t OAMDMA::read() {
  throw invalidRegisterOp("OAMDMA", "read");
}

PPUStateData PPU::dumpState() {
  PPUStateData d;
  d.clock = clock;
  d.frameCount = frameCount;
  d.scanLine = scanLine;
  d.latchValue = latchValue;
  return d;
}

PPU::PPU(Console& _console):
  log(Logger::getLogger("PPU", "ppu.log")),
  mem(_console),
  console(_console),
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
  latchValue = 0;
  nmiOccured = false;
  nmiPrevious = false;
  nmiDelay = 0;

  currentVram = 0;
  temporaryVram = 0;
  fineScroll = 0;
  writeToggle = false;
  
  clock = 0;
  scanLine = 0;
  isEvenScreen = true;
  frameCount = 0;

  nameTableByte = 0;
  attributeTableByte = 0;
  lowerTileByte = 0;
  higherTileByte = 0;
  backgroundData = 0; // 64 bits

  spriteCount = 0;
  log.setLevel(DEBUG);

}

/* PUBLIC FUNCTIONS */
void PPU::reset() {
  clock = 340;
  scanLine = 240;
  ppuctrl.write(0);
  ppumask.write(0);
  oamaddr.write(0);
}

bool PPU::getNmiOccured() { 
  return nmiOccured;
}

int PPU::getLatchValue() { 
  return latchValue; 
}

void PPU::setNmiStatus(bool occured, bool previous) {
  nmiOccured = occured;
  nmiPrevious = previous;
}

void PPU::setWriteToggle(bool status) { writeToggle = status; }

bool PPU::getWriteToggle() { return writeToggle; }

uint8_t PPU::getOamAddress() { return oamaddr.address; }

void PPU::incrementOamAddress() { oamaddr.write(oamaddr.address + 1); }

void PPU::setCurrentVram(uint16_t val){ currentVram = val; }

uint16_t PPU::getCurrentVram() { 
  return currentVram;
}

void PPU::setTemporaryVram(uint16_t val){ temporaryVram = val; }

uint16_t PPU::getTemporaryVram() { return temporaryVram; }

void PPU::setFineScroll(uint8_t val){ fineScroll = val; }

bool PPU::getIncrementFlag() { return ppuctrl.incrementFlag; }

PPUMemory& PPU::getMemory() { return mem; }

void PPU::uploadToOamdata(uint16_t begin, uint16_t end) {
  std::vector<uint8_t> buffer(end - begin);
  for (int i = 0; i < end - begin; i++)
    buffer[i] = console.getCpu().getMemory().read(begin + i);
  oamdata.upload(buffer); 
}

void PPU::makeCpuWait(int cycles) {
  console.getCpu().waitFor(cycles);
}

long PPU::getClock() { return clock; }
    
uint8_t PPU::readRegister(uint16_t address) {
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

void PPU::writeRegister(uint16_t address, uint8_t value) {
  latchValue = value;
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
void PPU::nmiChange() {
  bool nmi = ppuctrl.nmiFlag && nmiOccured;
  if (nmi && !nmiPrevious)
    nmiDelay = 15;
  nmiPrevious = nmi;
}

void PPU::nextScreen() {
isEvenScreen = !isEvenScreen;
frameCount++;
console.getEngine().render();
}

int nextScanLine(int current) {
  int next = current + 1;
  if (next > PPU::PRE_RENDER_SCAN_LINE) {
    next = 0;
  }
  return next;
}

void PPU::tick() {
  if (nmiDelay > 0) {
    nmiDelay--;
    if (nmiDelay == 0 && ppuctrl.nmiFlag && nmiOccured)
      console.getCpu().triggerNmi();
  }
  if ((ppumask.spritesFlag || ppumask.backgroundFlag)
    && !isEvenScreen && scanLine == 261 && clock == 339) {
    clock = 0;
    scanLine = 0;
    nextScreen();
    return;
  }

  clock++;
  if (clock == PPU::CLOCK_CYCLE) {
    clock = 0;
    scanLine = nextScanLine(scanLine);
    if (scanLine == 0) {
      nextScreen();
    }
  }
}

void PPU::clearVerticalBlank() {
  nmiOccured = false;
  nmiChange();
}

void PPU::setVerticalBlank() {
  nmiOccured = true;
  nmiChange();
}

void PPU::incrementHorizontalScroll() {
  if ((currentVram & 0x1f) == 31) {
    // set coarse x to 0, switch horizontal nametable
    currentVram &= 0xffe0;
    currentVram ^= 0x400;
  }
  else currentVram += 1; // increment coarse x
}

void PPU::incrementVerticalScroll() {
  if ((currentVram & 0x7000) != 0x7000)
    currentVram += 0x1000;
  else {
    currentVram &= 0x8fff;
    int coarseY = (currentVram & 0x3e0) >> 5;
    if (coarseY == 29) {
      coarseY = 0;
      currentVram ^= 0x800;
    }
    else if (coarseY == 31)
      coarseY = 0;
    else
      coarseY++;
    currentVram = (currentVram & 0xfc1f) | (coarseY << 5);
  }
}

void PPU::copyHorizontalScroll() {
  currentVram = (currentVram & 0xfbe0) | (temporaryVram & 0x041f);
}

void PPU::copyVerticalScroll() {
  currentVram = (currentVram & 0x841f) | (temporaryVram & 0x7be0);
}

int PPU::fetchSpriteGraphics(int num, int row) {
  /* Fetches the pixel data for sprite num, 
   * where row is the row WITHIN the sprite (0 = top) 
   * */
  uint8_t tileIndex = oamdata.data[num * 4 + 1];
  uint8_t attributes = oamdata.data[num * 4 + 2];
  bool verticalFlip = (attributes & 0x80) == 0x80;
  bool horizontalFlip = (attributes & 0x40) == 0x40;
  bool table;
  if (!ppuctrl.spriteSizeFlag) {
    if (verticalFlip) row = 7 - row;
    table = ppuctrl.spriteTableFlag;
  }
  else {
    if (verticalFlip) row = 15 - row;
    table = tileIndex & 1;
    tileIndex &= 0xfe;
    if (row > 7) {
      tileIndex++;
      row -= 8;
    }
  }
  uint16_t address = 0x1000 * table + 0x10 * tileIndex + row;
  uint8_t lowTileByte = mem.read(address);
  uint8_t highTileByte = mem.read(address + 8);
  // combine the data for 8 pixels
  uint8_t a, b, c;
  int _spriteGraphics = 0;
  a = (attributes & 0b11) << 2;
  for (int i = 0; i < 8; i++) {
    if (horizontalFlip) {
      b = (highTileByte & 1) << 1;
      c = (highTileByte & 1) << 0;
      lowTileByte >>= 1;
      highTileByte >>= 1;
    }
    else {
      b = (highTileByte & 0x80) >> 6;
      c = (lowTileByte & 0x80) >> 7;
      lowTileByte <<= 1;
      highTileByte <<= 1;
    }
    _spriteGraphics <<= 4;
    _spriteGraphics |= (a | b | c);
  }
  return _spriteGraphics;
}

// loadSpriteData fetches the sprite information for all sprites of the next
// scan line
void PPU::loadSpriteData() {
  int height = ppuctrl.spriteSizeFlag ? 16 : 8;
  int _spriteCount = 0;
  int x, y, attributeData, top, bottom;
  int lineToLoad = nextScanLine(scanLine);

  for (int i = 0; i < 64; i++) {
    x = oamdata.data[i * 4 + 3];
    attributeData = oamdata.data[i * 4 + 2];
    y = oamdata.data[i * 4];
    top = y;
    bottom = y + height;
    if ((lineToLoad < top) || (lineToLoad >= bottom)) continue;
    _spriteCount++;
    if (_spriteCount <= 8) {
      spriteGraphics[_spriteCount - 1] = fetchSpriteGraphics(i, lineToLoad - top);
      spritePositions[_spriteCount - 1] = x;
      spritePriorities[_spriteCount - 1] = (attributeData >> 5) & 1;
      spriteIndexes[_spriteCount - 1] = i;
    }
  }
  if (_spriteCount > 8) {
    // no rendering if we hit more than 8 sprites, but set overflow flag
    _spriteCount = 8;
    ppustatus.spriteOverflowFlag = true;
  }
  spriteCount = _spriteCount;
}

uint8_t PPU::getBackgroundPixel() {
  /* Gets pixel to render from the pre-loaded 64 bits of background data
   * The progressive shift is done in the main loop.
   * */
  if (!ppumask.backgroundFlag) return 0;
  uint32_t cycleData = backgroundData >> 32;
  if (frameCount > 20 * 60) {
    log.debug() << hex(backgroundData) << "cycle: " << hex(cycleData) << "\n";
  }
  uint8_t pixelData = (cycleData >> (7 - fineScroll) * 4) & 0xf; 
  return pixelData;
}

void PPU::loadBackgroundData() {
  /* Due to the fact that getBackgroundPixel() consumes data,
   * we need to refill it (or rather pre fill it in the previous cycle). 
   * One pixel needs 4 bits of info, total of 32 bits/cycle.
   * */
  uint32_t data = 0;
  uint8_t a, b, c;
  // attributeTableByte in fact contains the information twice (as there are
  // only 4 palettes, hence 4 bytes needed). Trim and pass to higher bits.
  a = (attributeTableByte & 0b11) << 2;
  if (frameCount > 20 * 60) {
    log.debug() << "attr: " << hex(attributeTableByte) 
                << "high: " << hex(higherTileByte)
                << "low: " << hex(lowerTileByte) << "\n";
  }
  for (int i = 0; i < 8; i ++) {
    b = (higherTileByte & 0x80) >> 6;
    c = (lowerTileByte & 0x80) >> 7;
    data <<= 4;
    higherTileByte <<= 1;
    lowerTileByte <<= 1;
    data |= (a | b | c);
  }
  if (frameCount > 20 * 60) {
    log.debug() << "new data: " << hex(data) << "\n";
  }
  backgroundData |= data;
}

void PPU::fetchNametableByte() {
  /* Given by 12 lowests bits of VRAM + $2000 offset */
  nameTableByte = mem.read(0x2000 + (currentVram & 0xfff));
}

void PPU::fetchAttributeTableByte() {
  /* To get the attribute table byte, we need to combine:
   *     - the 2 bits selecting the name table,
   *     - the three highest bits of the coarse Y scroll,
   *     - the three highest bits of the coarse X scroll
   * With a #23C0 offset.
   * */
  uint16_t address = 0x23c0;
  address |= currentVram & 0xc00;
  address |= (currentVram & 0x380) >> 4;
  address |= (currentVram & 0x1c) >> 2;
  attributeTableByte = mem.read(address);
  if (frameCount > 20 * 60) {
    log.debug() << "addr: " << hex(address) 
                << " vram: " << hex(currentVram)
                << " attr: " << hex(attributeTableByte) << "\n";
  }
}

void PPU::fetchLowerTileByte() {
  /*
   * To fetch a tile byte, combine:
   *  - table index(stored in PPUCTRL) * $1000
   *  - tile index (in the name table byte) * $10
   *  - fine Y scroll (3 highest bits of VRAM address)
   * */
  uint8_t fineY = (currentVram >> 12) & 0x7;
  uint8_t tableIndex = ppuctrl.backgroundTableFlag;
  uint8_t tileIndex = nameTableByte;
  uint16_t address = 0x1000 * tableIndex + 0x10 * tileIndex + fineY;
  lowerTileByte = mem.read(address);
}

void PPU::fetchHigherTileByte() {
  /* Read one byte above the lower tile byte */
  uint8_t fineY = (currentVram >> 12) & 0x7;
  uint8_t tableIndex = ppuctrl.backgroundTableFlag;
  uint8_t tileIndex = nameTableByte;
  uint16_t address = 0x1000 * tableIndex + 0x10 * tileIndex + fineY;
  higherTileByte = mem.read(address + 8);
}

void PPU::step() {
  tick();
  bool renderingEnabled = ppumask.backgroundFlag || ppumask.spritesFlag;
  bool isVisibleClock = (clock >= 1) && (clock <= 256);
  bool isVisibleLine = scanLine < PPU::POST_RENDER_SCAN_LINE;
  bool isPrerenderLine = scanLine == PPU::PRE_RENDER_SCAN_LINE;
  bool isNextOfPostrenderLine = scanLine == PPU::POST_RENDER_SCAN_LINE + 1;
  bool isFetchLine = isVisibleLine || isPrerenderLine;
  bool isFetchClock = isVisibleClock || ((clock <= 336) && (clock >= 321));
  if (renderingEnabled) {
    if (isVisibleLine && isVisibleClock) renderPixel();
    if (isFetchLine) {
      if (isFetchClock) {
        int _switch = clock % 8;
        // make sure we have 8 new bits every 2 ticks
        backgroundData <<= 4;
        switch (_switch) {
          case 0:
            incrementHorizontalScroll();
            loadBackgroundData();
            break;
          case 1:
            fetchNametableByte();
            break;
          case 3:
            fetchAttributeTableByte();
            break;
          case 5:
            fetchLowerTileByte();
            break;
          case 7:
            fetchHigherTileByte();
            break;
          default:
            break;
        }

      }
      if (clock == 256) incrementVerticalScroll();

      if (clock == 257) copyHorizontalScroll();

      if (clock == 257) loadSpriteData();
    }
    if (isPrerenderLine && (clock >= 280) && (clock <= 304))
      copyVerticalScroll();
  }
  if (isPrerenderLine && (clock == 1)) {
    clearVerticalBlank();
    ppustatus.spriteOverflowFlag = false;
    ppustatus.spriteZeroFlag = false;
  }

  // vertical blank is set after post-render line
  if (isNextOfPostrenderLine && (clock == 1)) setVerticalBlank();

}

SpritePixel PPU::getSpritePixel() {
  if (!ppumask.spritesFlag) return {0, 0};
  for (int i = 0; i < spriteCount; i++) {
    int offset = (clock - 1) - spritePositions[i];
    if ((offset < 0) || (offset > 7)) continue;
    uint8_t color = (spriteGraphics[i] >> (7 - offset) * 4) & 0xf;
    if (color % 4 == 0) continue; // sprite is transparent
    return {i, color};
  }
  return {0, 0};
}

void PPU::renderPixel() {
  int x = clock - 1;
  int y = scanLine;
  uint8_t background = getBackgroundPixel();
  int color;
  SpritePixel spritePix = getSpritePixel();
  if ((x < 8) && !ppumask.leftBackgroundFlag) background = 0;
  if ((x < 8) && !ppumask.leftSpritesFlag) spritePix.color = 0;
  bool bOpaque = background % 4 != 0;
  bool sOpaque = spritePix.color % 4 != 0;
  if (!sOpaque && !bOpaque) color = 0;
  else if (!bOpaque && sOpaque) color = spritePix.color | 0x10;
  else if (!sOpaque && bOpaque) color = background;
  else {
    if ((spriteIndexes[spritePix.num] == 0) && (x < 255))
      ppustatus.spriteZeroFlag = true;
    if (spritePriorities[spritePix.num] == 0)
      color = spritePix.color | 0x10;
    else
      color =  background;
  }
  uint8_t paletteInfo = mem.read(0x3f00 + color % 64);
  if (frameCount > 20 * 60) {
    log.debug() << "sprite: " << hex(spritePix.color) << "back: " << hex(background) << "\n";
    log.debug() << "(" << x << "," << y << ")" << ": " << hex(paletteInfo) << "\n";
  }
  console.getEngine().colorPixel(x, y, paletteInfo);
}
