#include "replay_interface.h"


ReplayInterface::ReplayInterface(InterfaceType t):
  screenStream("screen.log")
{
  target = IOInterface::newIOInterface(t);
}

bool ReplayInterface::shouldClose() {
  return isClose;
}

bool ReplayInterface::shouldReset() { return false; }

void ReplayInterface::render() {
  target->render();
}

void ReplayInterface::colorPixel(int x, int y, int palette) {
  uint8_t val = screenStream.read();
  if (val == utils::SCREENSTREAM_END) {
    isClose = true;
    return;
  }
  target->colorPixel(x, y, (int)val);
}

std::array<ButtonSet, 2> ReplayInterface::getButtons() {
  return target->getButtons();
}
