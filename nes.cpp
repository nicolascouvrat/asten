#include <string>

#include "console.h"
#include "logger.h"
#include "io_interface.h"


int main(int argc, char* argv[]) {
  Logger log = Logger::getLogger("main");
  if (argc != 2) {
    log.error() << "Oops, path to a .nes file was not provided\n";
    return -1;
  } 
  std::string path(argv[1]);
  Console console(path, InterfaceType::DEBUG_INTERFACE);
  while (console.isRunning()) {
    console.step();
  }
  return 0;
}
