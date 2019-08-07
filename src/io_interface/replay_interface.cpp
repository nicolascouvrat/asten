#include "replay_interface.h"

#include "streams.h"

ReplayInterface::ReplayInterface(InterfaceType t):
  target(IOInterface::newIOInterface(t)),
  screenStream("screen.log", utils::StreamMode::IN, IOInterface::WIDTH*IOInterface::HEIGHT),
  isClose(false)
{}

bool ReplayInterface::shouldClose() {
  return isClose;
}

bool ReplayInterface::shouldReset() { return false; }

void ReplayInterface::render() {
  target->render();
}

void ReplayInterface::colorPixel(int x, int y, int palette) {
  // Due to how the ppu (calling colorPixel) cycles several times for each cpu
  // (calling shouldClose()) cycle, it is possible that we try to render a few
  // more pixels before realizing that in fact we should be done.
  // Return straight away to avoid any problems.
  if (isClose) {
    return;
  }

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
