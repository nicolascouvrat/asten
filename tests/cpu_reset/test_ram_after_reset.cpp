#include "console.h"
#include "io_interface.h"

int main() {
  Console console("ram_after_reset.nes", InterfaceType::DEBUG_INTERFACE);
  while (console.isRunning()) {
    console.step();
  }
  return 0;
}
