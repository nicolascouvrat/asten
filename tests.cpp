#include <string>
#include <fstream>
#include <sstream>

#include "console.h"
#include "logger.h"
#include "nes_engine.h"


int test_CPU() {
  Logger log = Logger::getLogger("main");
  log.setLevel(DEBUG);
  Console console("roms/nestest.nes");
  CPU& c = console.getCpu();
  c.reset();
  c.debugSetPc(0xc000); // needed to start the test in autorun
  int counter = 0;
  CPUStateData benchmarkState, prevCpuState;
  std::string s;
  std::ifstream in("benchmark.txt");
  while (std::getline(in, s)) {
    if (s.front() == '#')
      // comment
      continue;
    std::istringstream iss(s);
    iss >> std::hex >> benchmarkState;
    CPUStateData cpuState = c.dumpState();
    if (!(benchmarkState == cpuState)) {
      log.debug() << "INSTRUCTION n=" << counter << "\n";
      log.debug() << std::setw(12) << "Should be: " << benchmarkState << "\n";
      log.debug() << std::setw(12) << "Got: " << cpuState << "\n";
      log.debug() << std::setw(12) << "Prev: " << prevCpuState << "\n";
      break;
    }
    prevCpuState = cpuState;
    console.step();
    counter++;
  }
  log.info() << "Ran " << counter << " instructions succesfully.\n";
  return 0;
}

int test_PPU() {
  Logger log = Logger::getLogger("main");
  log.setLevel(DEBUG);
  Console console("roms/color_test.nes");
  CPU& cpu =  console.getCpu();
  int counter =  0;
  // TODO: remove
  CPUStateData benchmarkState, prevCpuState;
  std::string s;
  std::ifstream in("cpuo_ps.txt");
  // while (true) {
  while (std::getline(in, s)) {
    std::istringstream iss(s);
    iss >> std::hex >> benchmarkState;
    CPUStateData cpuState = cpu.dumpState();
    if (!(benchmarkState == cpuState)) {
      log.debug() << "INSTRUCTION n=" << counter << "\n";
      log.debug() << std::setw(12) << "Should be: " << benchmarkState << "\n";
      log.debug() << std::setw(12) << "Got: " << cpuState << "\n";
      log.debug() << std::setw(12) << "Prev: " << prevCpuState << "\n";
      cpu.getMemory().debugDump(0x8170, 0x10, 0x10);
      break;
    }
    prevCpuState = cpuState;
    console.step();
    counter ++;
    log.debug() << counter << "\n";
  }
  return 0;
    
}

int integrationTest() {
  Console console("roms/ff3.nes");
  while (console.isRunning()) {
    console.step();
  }
  return 0;
}

int testEngine() {
  NesEngine engine;
  while (engine.isRunning()) {
    engine.render();
  }
  return 0;
}

int dummy_PPU() {
  Logger log = Logger::getLogger("main", "test.log");
  log.setLevel(DEBUG);
  Console console("roms/color_test.nes");
  int maxCycles = 37000;
  int counter =  0;
  while (counter < maxCycles) {
    console.step();
    counter ++;
    log.debug() << counter << "\n";
  }
  return 0;
}

int main(void) {
  // return test_PPU();
  return integrationTest();
}
