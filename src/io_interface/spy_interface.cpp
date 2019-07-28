#include "spy_interface.h"
#include <iostream>


void flushBuf(std::string& buf, std::ofstream& out) {
  std::cout << buf.length() << "\n";
  out << buf;
  out.flush();
  buf.resize(0);
}

SpyInterface::SpyInterface(InterfaceType t):
  screenOut("screen.log"), btnStream("buttons.log")
{
  target = IOInterface::newIOInterface(t);
  screenBuf.reserve(SpyInterface::BUF_SIZE);
  buttonsBuf.reserve(SpyInterface::BUF_SIZE);
}

bool SpyInterface::shouldClose() { 
  bool willClose = target->shouldClose();
  if (willClose) {
    screenBuf.push_back('Y');
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
    buttons[0].isEqual(currentButtons[0]) &&
    buttons[1].isEqual(currentButtons[1])
  ) {
    // the buttons did not change, increment counter
    identicalCount ++;
    return currentButtons;
  }

  // Write old buttons
  // TODO: support two button sets
  utils::ButtonsBuffer encoded = currentButtons[0].encode(identicalCount);
  btnStream.write(encoded);

  // Reset
  currentButtons = buttons;
  identicalCount = 1;
  return buttons;
}

void SpyInterface::maybeFlush(std::string& buf, std::ofstream& out) {
  if (buf.length() + 100 > SpyInterface::BUF_SIZE) {
    flushBuf(buf, out);
  }
}

