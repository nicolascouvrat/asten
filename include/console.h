#ifndef GUARD_CONSOLE_H
#define GUARD_CONSOLE_H


#include <string>

#include "cpu.h"
#include "ppu.h"
#include "logger.h"
#include "controller.h"
#include "io_interface.h"


class Mapper;
// Console is the top level of the NES emulator. It contains a reference to all
// the different parts.
class Console {
  public:
    // Creates a console that will run the .nes file at romPath, with an
    // IOInterface of type type. In the case where the interface is a type that
    // results in the creation of button and screen log files, these will be
    // fetched from (or saved at) btnLogPath and scrnLogPath respectively
    Console(std::string romPath, InterfaceType type, std::string btnLogPath, std::string scrnLogPath);
    Mapper *getMapper();
    CPU& getCpu();
    PPU& getPpu();
    Controller& getLeftController();
    Controller& getRightController();
    IOInterface* getInterface();
    // step simulates one clock cycle of the console
    void step();
    // isRunning returns true if the console is currently active
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
