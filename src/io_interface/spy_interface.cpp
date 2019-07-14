#include "spy_interface.h"


SpyInterface::SpyInterface(InterfaceType t):
  colors({0}), out("game.log")
{
  target = IOInterface::newIOInterface(t);
  buf.reserve(SpyInterface::BUF_SIZE);
}

bool SpyInterface::shouldClose() { return target->shouldClose(); }

bool SpyInterface::shouldReset() { return target->shouldReset(); }

void SpyInterface::render() {
  target->render();
}

void SpyInterface::colorPixel(int x, int y, int palette) {
  buf.push_back((char)palette);
  maybeFlush();
  target->colorPixel(x, y, palette);
}

std::array<ButtonSet, 2> SpyInterface::getButtons() {
  auto buttons = target->getButtons();
  if (
    !buttons[0].isEqual(currentButtons[0]) ||
    !buttons[1].isEqual(currentButtons[1])
  ) {
    auto encoded = buttons[0].encode(identicalCount);
    buf.append(encoded);
    maybeFlush();
    identicalCount = 0;
    currentButtons = buttons;
  } else {
    identicalCount++;
  }
  return buttons;
}

void SpyInterface::maybeFlush() {
  if (buf.length() + 1 > SpyInterface::BUF_SIZE) {
    out << buf;
    buf.resize(0);
  }
}
