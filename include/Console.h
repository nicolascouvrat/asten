#ifndef GUARD_CONSOLE_H
#define GUARD_CONSOLE_H


#include <string>

#include "cpu.h"
#include "ppu.h"
#include "logger.h"
#include "nes_engine.h"
#include "controller.h"


class Mapper;
class Console {
  public:
    Console(std::string);
    Mapper *getMapper();
    CPU& getCpu();
    PPU& getPpu();
    Controller& getLeftController();
    Controller& getRightController();
    NesEngine& getEngine();
    void step();
    bool isRunning();
  private:
    Logger log;
    CPU cpu;
    PPU ppu;
    Controller leftController;
    Controller rightController;
    NesEngine engine;
    Mapper *mapper;
};

#endif
