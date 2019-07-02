#include "io_interface.h"
#include "classic_interface.h"

IOInterface* IOInterface::newIOInterface(InterfaceType type) {
  switch (type) {
    case CLASSIC:
      return new ClassicInterface();
  }
}
