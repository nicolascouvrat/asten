#include "Console.h"
#include "Logger.h"
#include "Mapper.h"

#include <iostream>
#include <string>
#include <iterator>
// file parsing
#include <fstream>
#include <sstream>
#include <iomanip>


using std::string;  using std::cout;    using std::endl;
using std::ostream; using std::iterator;


namespace {
    Logger log = Logger::get_logger("main").set_level(DEBUG);
}

int test_CPU() {
    Console console("nestest.nes");
    CPU c = console.get_cpu();
    c.reset();
    int counter = 0;
    CPUStateData benchmark_state, cpu_state, prev_cpu_state;
    std::string s;
    std::ifstream in("benchmark.txt");
    while (std::getline(in, s)) {
        if (s.front() == '#')
            // comment
            continue;
        std::istringstream iss(s);
        iss >> std::hex >> benchmark_state;
        cpu_state << c;
        if (!(benchmark_state == cpu_state)) {
            log.debug() << "INSTRUCTION n=" << counter << "\n";
            log.debug() << std::setw(12) << "Should be: " << benchmark_state << "\n";
            log.debug() << std::setw(12) << "Got: " << cpu_state << "\n";
            log.debug() << std::setw(12) << "Prev: " << prev_cpu_state << "\n";
            c.get_memory().debug_dump(0x0200, 0x100, 0x10);
            break;
        }
        prev_cpu_state = cpu_state;
        c.step();
        counter++;
    }
    return 0;
}

int test_Mapper() {
    Console c = Console("nestest.nes");
    return 0;
}

int main(void) {
    return test_Mapper();
}
