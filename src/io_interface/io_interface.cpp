#include "io_interface.h"
#include "classic_interface.h"

IOInterface* IOInterface::newIOInterface() {
  return new ClassicInterface();
}
