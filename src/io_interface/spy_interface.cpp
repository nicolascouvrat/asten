#include "spy_interface.h"


SpyInterface::SpyInterface(InterfaceType t):
  colors({0}),
  out("game.log")
{
  target = IOInterface::newIOInterface(t);
  buf.reserve(SpyInterface::BUF_SIZE);
}

bool SpyInterface::shouldClose() { return target->shouldClose(); }

bool SpyInterface::shouldReset() { return target->shouldReset(); }

void SpyInterface::render() {
  maybeFlush();
  target->render();
}

void SpyInterface::colorPixel(int x, int y, int palette) {
  int index = y * IOInterface::WIDTH + x;
  colors[index] = (char)palette;
  target->colorPixel(x, y, palette);
}

std::array<ButtonSet, 2> SpyInterface::getButtons() {
  return target->getButtons();
}

void SpyInterface::maybeFlush() {
  for (auto i = colors.begin(); i != colors.end(); i++) {
    buf.push_back(*i);
    if (buf.length() + 1 > SpyInterface::BUF_SIZE) {
      out << buf;
      buf.resize(0);
    }
  }
  buf.push_back('\n');
}
