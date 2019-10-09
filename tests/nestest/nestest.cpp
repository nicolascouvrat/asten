#include "console.h"
#include "io_interface.h"

int main() {
  Console console(
    "nestest.nes",
    InterfaceType::DEBUG_INTERFACE,
    "nestest.btn",
    "nestest.scrn"
  );

  while (console.isRunning()) {
    console.step();
  }

  return 0;
}

