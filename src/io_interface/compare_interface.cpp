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
  loadNextReset();
}

bool CompareInterface::shouldClose() { return isDone; }

bool CompareInterface::shouldReset() {
  if (remainingRstCount == 0) {
    loadNextReset();
  }

  remainingRstCount--;
  return currentReset;
}

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

// loadNextButtons will charge the next buttonset in the queue
// 
// It can be safely used on a empty queue, in which case it will load the zero
// value of ButtonBuffer
void CompareInterface::loadNextButtons() {
  auto next = nextButtons.front();
  nextButtons.pop();
  std::cout << "loading: " << next;
  remainingCount = currentButtons[0].unmarshal(next);
}

// loadNextReset will charge the next reset in the queue
// 
// It can be safely used on a empty queue, in which case it will load the zero
// value of ResetBuffer
void CompareInterface::loadNextReset() {
  auto next = nextResets.front();
  nextResets.pop();
  std::cout << "loading: " << next;
  remainingRstCount = next.count;
  currentReset = next.reset;

}

