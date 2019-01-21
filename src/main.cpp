#include "Cpu.h"
#include "BinaryReader.h"
#include "Logger.h"

#include <iostream>
#include <string>
#include <iterator>


using std::string;  using std::cout;    using std::endl;
using std::ostream; using std::iterator;


namespace {
    Logger log = Logger::get_logger("main").set_level(DEBUG);
}

int main(void) {
    BinaryReader r("test.bin");
    CPU c;
    // load program in memory at pc start location
    c.get_memory().load(0x6000, r.read_bytes());
    uint16_t steps = 150;
    for (uint16_t i = 1; i < steps; i++) {
        c.step();
    }
    c.get_memory().debug_dump(0x200, 20, 10);
    uint16_t t = 0x1150;
    log.debug() << hex((uint8_t) t);
}
