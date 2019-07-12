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

std::string EncodeButtonSet(ButtonSet bs) {
  std::string s;

  s += BUTTONS_START;

  if (bs.A) s.push_back(A_CODE);
  if (bs.B) s.push_back(B_CODE);
  if (bs.SELECT) s.push_back(SELECT_CODE);
  if (bs.START) s.push_back(START_CODE);
  if (bs.UP) s.push_back(UP_CODE);
  if (bs.DOWN) s.push_back(DOWN_CODE);
  if (bs.LEFT) s.push_back(LEFT_CODE);
  if (bs.RIGHT) s.push_back(RIGHT_CODE);

  // avoid writing useless stuff if we had nothing
  if (s.length() == 1) return "";

  s += BUTTONS_END;

  return s;
}

ButtonSet DecodeButtonSet(std::string in) {
  // Test correct encoding
  if (in[0] != BUTTONS_START || in[in.length() - 1] != BUTTONS_END) {
      throw std::runtime_error("invalid button set encoding");
  }

  ButtonSet bs;
  for (auto i = in.begin(); i != in.end(); i++) {
    switch(*i) {
      case A_CODE:
        bs.A = true;
        break;
      case B_CODE:
        bs.B = true;
        break;
      case SELECT_CODE:
        bs.SELECT = true;
        break;
      case START_CODE:
        bs.START = true;
        break;
      case UP_CODE:
        bs.UP = true;
        break;
      case DOWN_CODE:
        bs.DOWN = true;
        break;
      case LEFT_CODE:
        bs.LEFT = true;
        break;
      case RIGHT_CODE:
        bs.RIGHT = true;
        break;
    }
  }
  return bs;
}
