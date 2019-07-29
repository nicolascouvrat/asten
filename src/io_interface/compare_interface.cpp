#include "compare_interface.h"
#include <iostream>


CompareInterface::CompareInterface(InterfaceType t):
  screenStream("screen.log"),
  btnStream("buttons.log"),
  currentButtons({0})
{
  target = IOInterface::newIOInterface(t);
  btnStream.readAll(nextButtons, nextResets);
  std::cout << nextButtons.size() << "\n";
  loadNextButtons();
}

bool CompareInterface::shouldClose() { return isDone; }

bool CompareInterface::shouldReset() { return target->shouldReset(); }

void CompareInterface::render() {
  target->render();
}

void CompareInterface::colorPixel(int x, int y, int palette) {
  uint8_t val = screenStream.read();
  if (val == utils::SCREENSTREAM_END) {
    isDone = true;
    return;
  }
  if ((int)val != palette) {
    // TODO: this probably does not work for games where random numbers are
    // involved...
    // throw std::runtime_error("OOPS");
  }
  target->colorPixel(x, y, palette);
}

std::array<ButtonSet, 2> CompareInterface::getButtons() {
  if (remainingCount == 0) {
    loadNextButtons();
  }
  remainingCount--;
  return currentButtons;
}

void CompareInterface::loadNextButtons() {
  auto next = nextButtons.front();
  nextButtons.pop();
  std::cout << "loading: " << next;
  remainingCount = currentButtons[0].unmarshal(next);
}

