#include "console.h"

#include "mapper.h"
#include "io_interface.h"


Mapper *Console::getMapper() { return mapper; }

CPU& Console::getCpu() { return cpu; }

PPU& Console::getPpu() { return ppu; }

IOInterface* Console::getInterface() { return interface; }

Controller& Console::getLeftController() { return leftController; }

Controller& Console::getRightController() { return rightController; }

Console::Console(std::string name): 
  log(Logger::getLogger("Console")),
  cpu(*this), ppu(*this),
  mapper(Mapper::fromNesFile(*this, name)),
  interface(IOInterface::newIOInterface())
{
  log.setLevel(DEBUG);
  cpu.reset();
  ppu.reset();
}

void Console::step() {
  if (interface->shouldReset()) {
    cpu.reset();
  }
  auto buttons = interface->getButtons();
  leftController.set(buttons[0]);
  rightController.set(buttons[1]);
  long cpuSteps = cpu.step();
  cpu.fastForwardClock(2 * cpuSteps);
  for (int i = 0; i < 3 * cpuSteps; i++)
    ppu.step();
}

bool Console::isRunning() {
  return !interface->shouldClose();
}
    

