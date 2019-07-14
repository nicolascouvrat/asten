#include "io_interface.h"
#include "classic_interface.h"
#include "spy_interface.h"
#include "replay_interface.h"
#include "compare_interface.h"

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
    case DEBUG_INTERFACE:
      return new CompareInterface(InterfaceType::CLASSIC);
  }
}

std::string ButtonSet::encode(long count) {
  std::string s;

  s += BUTTONS_START;

  s += std::to_string(count);

  if (A) s.push_back(A_CODE);
  if (B) s.push_back(B_CODE);
  if (SELECT) s.push_back(SELECT_CODE);
  if (START) s.push_back(START_CODE);
  if (UP) s.push_back(UP_CODE);
  if (DOWN) s.push_back(DOWN_CODE);
  if (LEFT) s.push_back(LEFT_CODE);
  if (RIGHT) s.push_back(RIGHT_CODE);

  s += BUTTONS_END;

  return s;
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

long DecodeButtonSet(std::string in, ButtonSet* bs) {
  // Test correct encoding
  if (in[0] != BUTTONS_START || in[in.length() - 1] != BUTTONS_END) {
      throw std::runtime_error("invalid button set encoding");
  }

  // reset all buttons
  bs->A = false;
  bs->B = false;
  bs->SELECT = false;
  bs->START = false;
  bs->UP = false;
  bs->LEFT = false;
  bs->RIGHT = false;
  bs->DOWN = false;

  long counter = 0;

  for (auto i = in.begin(); i != in.end(); i++) {
    if (('0' <= *i) && (*i <= '9')) {
      counter *= 10;
      counter += *i - '0';
      continue;
    } 

    switch(*i) {
      case A_CODE:
        bs->A = true;
        break;
      case B_CODE:
        bs->B = true;
        break;
      case SELECT_CODE:
        bs->SELECT = true;
        break;
      case START_CODE:
        bs->START = true;
        break;
      case UP_CODE:
        bs->UP = true;
        break;
      case DOWN_CODE:
        bs->DOWN = true;
        break;
      case LEFT_CODE:
        bs->LEFT = true;
        break;
      case RIGHT_CODE:
        bs->RIGHT = true;
        break;
    }
  }
  return counter;
}
