#include "spy_interface.h"

SpyInterface::SpyInterface(InterfaceType t) {
  target = IOInterface::newIOInterface(t);
}

bool SpyInterface::shouldClose() { return target->shouldClose(); }

bool SpyInterface::shouldReset() { return target->shouldReset(); }

void SpyInterface::render() {
  target->render();
}

void SpyInterface::colorPixel(int x, int y, int palette) {
  target->colorPixel(x, y, palette);
}

std::array<ButtonSet, 2> SpyInterface::getButtons() {
  return target->getButtons();
}
