#include "compare_interface.h"

#include <iostream>

#include "streams.h"

std::runtime_error compareError() {
  return std::runtime_error("comparison error");
}

CompareInterface::CompareInterface(InterfaceType t, std::string btnLogPath, std::string scrnLogPath):
  target(IOInterface::newIOInterface(t, "", "")),
  btnStream(btnLogPath, utils::StreamMode::IN),
  screenStream(scrnLogPath, utils::StreamMode::IN, IOInterface::WIDTH*IOInterface::HEIGHT),
  remainingCount(0), currentButtons({0}), 
  remainingRstCount(0), currentReset(false), isDone(false)
{
  // Read everything from the button stream up front, then load the first button
  // and first reset
  btnStream.readAll(nextButtons, nextResets);
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
  // Due to how the ppu (calling colorPixel) cycles several times for each cpu
  // (calling shouldClose()) cycle, it is possible that we try to render a few
  // more pixels before realizing that in fact we should be done.
  // Return straight away to avoid any problems.
  if (isDone) {
    return;
  }

  uint8_t val = screenStream.read();
  if (val == utils::SCREENSTREAM_END) {
    isDone = true;
    return;
  }
  if ((int)val != palette) {
    // XXX: This simple equality test is fine for fully deterministic programs
    // (i.e. tests) but will fail otherwise.
    throw compareError();
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
void CompareInterface::loadNextButtons() {
  if (nextButtons.empty()) {
    return;
  }

  auto next = nextButtons.front();
  nextButtons.pop();
  std::cout << "loading: " << next;
  // TODO: handle second button
  remainingCount = currentButtons[0].unmarshal(next);
}

// loadNextReset will charge the next reset in the queue
void CompareInterface::loadNextReset() {
  if (nextResets.empty()) {
    return;
  }

  auto next = nextResets.front();
  nextResets.pop();
  std::cout << "loading: " << next;
  remainingRstCount = next.count;
  currentReset = next.reset;

}

