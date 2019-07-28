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
}

bool SpyInterface::shouldClose() { 
  bool willClose = target->shouldClose();
  if (willClose) {
    screenBuf.push_back('Y');
    flushBuf(screenBuf, screenOut);
    btnStream.flush();
  }
  return willClose;
}

bool SpyInterface::shouldReset() {
  bool reset = target->shouldReset();

  if (reset == currentReset) {
    // reset value did not change, increment counter
    identicalRstCount++;
    return currentReset;
  }

  // Write old buttons
  utils::ResetBuffer encoded;
  encoded.count = identicalRstCount;
  encoded.reset = currentReset;
  std::cout << encoded;
  btnStream.write(encoded);

  // Reset
  currentReset = reset;
  identicalRstCount = 1;

  return currentReset;
}

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
  std::cout << encoded;
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

