#include "Console.h"
#include "Mapper.h"

Mapper *Console::get_mapper() {
    return mapper;
}

CPU& Console::get_cpu() {
    return cpu;
}

Console::Console(std::string name): 
    cpu(*this), mapper(Mapper::from_nes_file(name)) 
{}

void Console::step() {
    long cpu_steps = cpu.step();
    cpu.wait_for(2 * cpu_steps);
}
    

