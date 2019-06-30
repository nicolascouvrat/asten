#include "io_interface.h"
#include "nes_engine.h"

IOInterface* IOInterface::newIOInterface() {
  return new NesEngine();
}
