#include "Console.h"
#include "Mapper.h"


Mapper *Console::get_mapper() { return mapper; }

CPU& Console::get_cpu() { return cpu; }

PPU& Console::get_ppu() { return ppu; }

NesEngine& Console::get_engine() { return engine; }

Controller& Console::get_left_controller() { return left_controller; }

Controller& Console::get_right_controller() { return right_controller; }

Console::Console(std::string name): 
    cpu(*this), mapper(Mapper::from_nes_file(name)), ppu(*this),
    log(Logger::get_logger("Console"))
{
    log.set_level(DEBUG);
    cpu.reset();
    ppu.reset();
}

void Console::step() {
    auto buttons = engine.getButtons();
    left_controller.set(buttons);
    long cpu_steps = cpu.step();
    cpu.fast_forward_clock(2 * cpu_steps);
    for (int i = 0; i < 3 * cpu_steps; i++)
        ppu.step();
}

bool Console::isRunning() {
  return engine.isRunning();
}
    

