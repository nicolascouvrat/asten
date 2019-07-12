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
  for (auto i = colors.begin(); i != colors.end(); i++) {
    buf.push_back(*i);
    maybeFlush();
  }
  buf.push_back('\n');
  target->render();
}

void SpyInterface::colorPixel(int x, int y, int palette) {
  int index = y * IOInterface::WIDTH + x;
  colors[index] = (char)palette;
  target->colorPixel(x, y, palette);
}

std::array<ButtonSet, 2> SpyInterface::getButtons() {
  auto buttons = target->getButtons();
  auto encoded = EncodeButtonSet(buttons[0]);
  buf.append(encoded);
  maybeFlush();
  return buttons;
}

void SpyInterface::maybeFlush() {
  if (buf.length() + 1 > SpyInterface::BUF_SIZE) {
    out << buf;
    buf.resize(0);
  }
}
