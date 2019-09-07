#include "console.h"
#include "io_interface.h"

int main() {
  Console console(
    "ram_after_reset.nes",
    InterfaceType::DEBUG_INTERFACE,
    "ram_after_reset.btn",
    "ram_after_reset.scrn"
  );

  while (console.isRunning()) {
    console.step();
  }

  return 0;
}
