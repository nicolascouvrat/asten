#include "spy_interface.h"

#include <iostream>

#include "streams.h"


SpyInterface::SpyInterface(InterfaceType t):
  screenStream("screen.log", utils::StreamMode::OUT), 
  btnStream("buttons.log", utils::StreamMode::OUT),
  currentButtons({0}), currentReset(false)
{
  target = IOInterface::newIOInterface(t);
}

bool SpyInterface::shouldClose() { 
  bool willClose = target->shouldClose();
  if (willClose) {
    writeCurrentButtons();
    writeCurrentReset();
    btnStream.close();
    screenStream.close();
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
  writeCurrentReset();

  // Reset
  currentReset = reset;
  identicalRstCount = 1;

  return currentReset;
}

void SpyInterface::render() {
  target->render();
}

void SpyInterface::colorPixel(int x, int y, int palette) {
  screenStream.write(palette);
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
  writeCurrentButtons();

  // Reset
  currentButtons = buttons;
  identicalCount = 1;
  return buttons;
}

void SpyInterface::writeCurrentButtons() {
  utils::ButtonsBuffer encoded = currentButtons[0].marshal(identicalCount);
  std::cout << encoded;
  btnStream.write(encoded);
}

void SpyInterface::writeCurrentReset() {
  std::cout << currentReset;
  utils::ResetBuffer encoded;
  encoded.count = identicalRstCount;
  encoded.reset = currentReset;
  std::cout << encoded;
  btnStream.write(encoded);
}
