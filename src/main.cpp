#include "Console.h"
#include "Logger.h"

#include <string>
#include <fstream>


int test_CPU() {
    Logger log = Logger::get_logger("main");
    log.set_level(DEBUG);
    Console console("nestest.nes");
    CPU& c = console.get_cpu();
    c.reset();
    c.debug_set_pc(0xc000); // needed to start the test in autorun
    int counter = 0;
    CPUStateData benchmark_state, prev_cpu_state;
    std::string s;
    std::ifstream in("benchmark.txt");
    while (std::getline(in, s)) {
        if (s.front() == '#')
            // comment
            continue;
        std::istringstream iss(s);
        iss >> std::hex >> benchmark_state;
        CPUStateData cpu_state = c.dump_state();
        if (!(benchmark_state == cpu_state)) {
            log.debug() << "INSTRUCTION n=" << counter << "\n";
            log.debug() << std::setw(12) << "Should be: " << benchmark_state << "\n";
            log.debug() << std::setw(12) << "Got: " << cpu_state << "\n";
            log.debug() << std::setw(12) << "Prev: " << prev_cpu_state << "\n";
            break;
        }
        prev_cpu_state = cpu_state;
        console.step();
        counter++;
    }
    log.info() << "Ran " << counter << " instructions succesfully.\n";
    return 0;
}

int main(void) {
    return test_CPU();
}
