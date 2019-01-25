#ifndef GUARD_CONSOLE_H
#define GUARD_CONSOLE_H


#include <string>
#include "Cpu.h"


class Mapper;
class Console {
    public:
        Console(std::string);
        Mapper *get_mapper();
        CPU& get_cpu();
        void step();
    private:
        CPU cpu;
        Mapper *mapper;
};

#endif
