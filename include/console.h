#ifndef GUARD_CONSOLE_H
#define GUARD_CONSOLE_H


#include <string>

#include "cpu.h"
#include "ppu.h"
#include "logger.h"
#include "controller.h"


class Mapper;
class IOInterface;
class Console {
  public:
    Console(std::string);
    Mapper *getMapper();
    CPU& getCpu();
    PPU& getPpu();
    Controller& getLeftController();
    Controller& getRightController();
    IOInterface* getInterface();
    void step();
    bool isRunning();
  private:
    Logger log;
    CPU cpu;
    PPU ppu;
    Controller leftController;
    Controller rightController;
    Mapper *mapper;
    IOInterface *interface;
};

#endif
