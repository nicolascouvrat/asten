#include "console.h"

#include "mapper.h"


Mapper *Console::getMapper() { return mapper; }

CPU& Console::getCpu() { return cpu; }

PPU& Console::getPpu() { return ppu; }

NesEngine& Console::getEngine() { return engine; }

Controller& Console::getLeftController() { return leftController; }

Controller& Console::getRightController() { return rightController; }

Console::Console(std::string name): 
  log(Logger::getLogger("Console")),
  cpu(*this), ppu(*this),
  mapper(Mapper::fromNesFile(*this, name))
{
  log.setLevel(DEBUG);
  cpu.reset();
  ppu.reset();
}

void Console::step() {
  if (engine.shouldReset()) {
    cpu.reset();
  }
  auto buttons = engine.getButtons();
  leftController.set(buttons);
  long cpuSteps = cpu.step();
  cpu.fastForwardClock(2 * cpuSteps);
  for (int i = 0; i < 3 * cpuSteps; i++)
    ppu.step();
}

bool Console::isRunning() {
  return engine.isRunning();
}
    

