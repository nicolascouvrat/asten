#ifndef GUARD_CONSOLE_H
#define GUARD_CONSOLE_H


#include <string>
#include "Cpu.h"
#include "Ppu.h"
#include "Logger.h"
#include "nes_engine.h"
#include "Controller.h"


class Mapper;
class Console {
    public:
        Console(std::string);
        Mapper *get_mapper();
        CPU& get_cpu();
        PPU& get_ppu();
        Controller& get_left_controller();
        Controller& get_right_controller();
        NesEngine& get_engine();
        void step();
        bool isRunning();
    private:
        Logger log;
        CPU cpu;
        PPU ppu;
        Controller left_controller;
        Controller right_controller;
        NesEngine engine;
        Mapper *mapper;
};

#endif
