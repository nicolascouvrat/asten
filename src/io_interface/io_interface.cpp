#include "io_interface.h"
#include "classic_interface.h"
#include "spy_interface.h"
#include "replay_interface.h"

IOInterface* IOInterface::newIOInterface(InterfaceType type) {
  switch (type) {
    case CLASSIC:
      return new ClassicInterface();
    case SINK:
      return new IOSink();
    case MONITOR:
      return new SpyInterface(InterfaceType::CLASSIC);
    case REPLAY:
      return new ReplayInterface(InterfaceType::CLASSIC);
  }
}
