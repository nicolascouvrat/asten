#include "spy_interface.h"
#include <iostream>


void flushBuf(std::string& buf, std::ofstream& out) {
  std::cout << buf.length() << "\n";
  out << buf;
  out.flush();
  buf.resize(0);
}

SpyInterface::SpyInterface(InterfaceType t):
  screenOut("screen.log"), buttonsOut("buttons.log")
{
  target = IOInterface::newIOInterface(t);
  screenBuf.reserve(SpyInterface::BUF_SIZE);
  buttonsBuf.reserve(SpyInterface::BUF_SIZE);
}

bool SpyInterface::shouldClose() { 
  bool willClose = target->shouldClose();
  if (willClose) {
    flushBuf(screenBuf, screenOut);
    flushBuf(buttonsBuf, buttonsOut);
  }
  return willClose;
}

bool SpyInterface::shouldReset() { return target->shouldReset(); }

void SpyInterface::render() {
  target->render();
}

void SpyInterface::colorPixel(int x, int y, int palette) {
  screenBuf.push_back((char)palette);
  maybeFlush(screenBuf, screenOut);
  target->colorPixel(x, y, palette);
}

std::array<ButtonSet, 2> SpyInterface::getButtons() {
  auto buttons = target->getButtons();
  if (
    !buttons[0].isEqual(currentButtons[0]) ||
    !buttons[1].isEqual(currentButtons[1])
  ) {
    auto encoded = buttons[0].encode(identicalCount);
    buttonsBuf.append(encoded);
    maybeFlush(buttonsBuf, buttonsOut);
    identicalCount = 0;
    currentButtons = buttons;
  } else {
    identicalCount++;
  }
  return buttons;
}

void SpyInterface::maybeFlush(std::string& buf, std::ofstream& out) {
  if (buf.length() + 100 > SpyInterface::BUF_SIZE) {
    flushBuf(buf, out);
  }
}

