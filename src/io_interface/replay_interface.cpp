#include "replay_interface.h"


ReplayInterface::ReplayInterface(InterfaceType t):
  in("game.log")
{
  target = IOInterface::newIOInterface(t);
}

bool ReplayInterface::shouldClose() { return done; }

bool ReplayInterface::shouldReset() { return target->shouldReset(); }

void ReplayInterface::render() {
  target->render();
}

void ReplayInterface::colorPixel(int x, int y, int palette) {
  char cin;
  in.get(cin);
  if (cin == EOF) {
    done = true;
    return;
  }
  target->colorPixel(x, y, (int)cin);
}

std::array<ButtonSet, 2> ReplayInterface::getButtons() {
  return target->getButtons();
}
