#include "spy_interface.h"
#include <iostream>


SpyInterface::SpyInterface(InterfaceType t):
  screenStream("screen.log"), btnStream("buttons.log")
{
  target = IOInterface::newIOInterface(t);
}

bool SpyInterface::shouldClose() { 
  bool willClose = target->shouldClose();
  if (willClose) {
    screenStream.close();
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
  utils::ButtonsBuffer encoded = currentButtons[0].encode(identicalCount);
  std::cout << encoded;
  btnStream.write(encoded);

  // Reset
  currentButtons = buttons;
  identicalCount = 1;
  return buttons;
}
