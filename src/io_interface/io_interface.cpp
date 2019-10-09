#include "io_interface.h"
#include "classic_interface.h"
#include "spy_interface.h"
#include "replay_interface.h"
#include "compare_interface.h"

IOInterface* IOInterface::newIOInterface(InterfaceType type, std::string btnLogPath, std::string scrnLogPath) {
  switch (type) {
    case CLASSIC:
      return new ClassicInterface();
    case SINK:
      return new IOSink();
    case MONITOR:
      return new SpyInterface(InterfaceType::CLASSIC, btnLogPath, scrnLogPath);
    case REPLAY:
      return new ReplayInterface(InterfaceType::CLASSIC, btnLogPath, scrnLogPath);
    case DEBUG_INTERFACE:
      return new CompareInterface(InterfaceType::SINK, btnLogPath, scrnLogPath);
  }
}

utils::ButtonsBuffer ButtonSet::marshal(long count) {
  bool buttonsOrdered[8] = {
    A, B, SELECT, START, UP, DOWN, LEFT, RIGHT,
  };

  uint8_t encoded;
  for (int i = 0; i < 7; i++) {
    encoded |= buttonsOrdered[i];
    encoded <<= 1;
  }

  encoded |= buttonsOrdered[7]; 

  utils::ButtonsBuffer buf;
  buf.count = count;
  buf.buttons = encoded;
  
  return buf;
}

long ButtonSet::unmarshal(utils::ButtonsBuffer& buf) {
  RIGHT = (buf.buttons >> 0) & 1;
  LEFT = (buf.buttons >> 1) & 1;
  DOWN = (buf.buttons >> 2) & 1;
  UP = (buf.buttons >> 3) & 1;
  START = (buf.buttons >> 4) & 1;
  SELECT = (buf.buttons >> 5) & 1;
  B = (buf.buttons >> 6) & 1;
  A = (buf.buttons >> 7) & 1;
  return buf.count;
}

bool ButtonSet::isEqual(ButtonSet bs) {
  return A == bs.A &&
    B == bs.B &&
    SELECT == bs.SELECT &&
    START == bs.START &&
    UP == bs.UP &&
    DOWN == bs.DOWN &&
    LEFT == bs.LEFT &&
    RIGHT == bs.RIGHT;
}
