#include "Console.h"
#include "Logger.h"
#include "nes_engine.h"

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

int test_PPU() {
    Logger log = Logger::get_logger("main");
    log.set_level(DEBUG);
    Console console("color_test.nes");
    CPU& cpu =  console.get_cpu();
    int counter =  0;
    // TODO: remove
    CPUStateData benchmark_state, prev_cpu_state;
    std::string s;
    std::ifstream in("cpu_ops.txt");
    // while (true) {
    while (std::getline(in, s)) {
        std::istringstream iss(s);
        iss >> std::hex >> benchmark_state;
        CPUStateData cpu_state = cpu.dump_state();
        if (!(benchmark_state == cpu_state)) {
            log.debug() << "INSTRUCTION n=" << counter << "\n";
            log.debug() << std::setw(12) << "Should be: " << benchmark_state << "\n";
            log.debug() << std::setw(12) << "Got: " << cpu_state << "\n";
            log.debug() << std::setw(12) << "Prev: " << prev_cpu_state << "\n";
            cpu.get_memory().debug_dump(0x8170, 0x10, 0x10);
            break;
        }
        prev_cpu_state = cpu_state;
        console.step();
        counter ++;
        log.debug() << counter << "\n";
    }
    return 0;
    
}

int integration_test() {
  Console console("dk.nes");
  while (console.isRunning()) {
    console.step();
  }
  return 0;
}

int test_engine() {
  NesEngine engine;
  while (engine.isRunning()) {
    engine.render();
  }
  return 0;
}

int dummy_PPU() {
    Logger log = Logger::get_logger("main", "test.log");
    log.set_level(DEBUG);
    Console console("color_test.nes");
    int max_cycles = 37000;
    int counter =  0;
    while (counter < max_cycles) {
        console.step();
        counter ++;
        log.debug() << counter << "\n";
    }
    return 0;
}

int main(void) {
    // return test_PPU();
    return integration_test();
}
