#include "compare_interface.h"
#include <iostream>


CompareInterface::CompareInterface(InterfaceType t):
  in("game.log"),
  currentButtons({0})
{
  target = IOInterface::newIOInterface(t);
}

bool CompareInterface::shouldClose() { return target->shouldClose(); }

bool CompareInterface::shouldReset() { return target->shouldReset(); }

void CompareInterface::render() {
  target->render();
}

void CompareInterface::colorPixel(int x, int y, int palette) {
  // just drop it
  char c = in.peek();
  if (c == BUTTONS_START) {
    std::cout << "WHAT\n";
    loadNextButtons();
    in.get(c);
  } else {
    in.get(c);
    // in.ignore();
  }
    target->colorPixel(x, y, (int)c);
}

std::array<ButtonSet, 2> CompareInterface::getButtons() {
  // if we did not already buffer some buttons
  char c = in.peek();
  if (c == BUTTONS_START) {
    loadNextButtons();
  }

  if (nextButtons.size() == 0) {
    currentCount ++;
    return currentButtons;
  }

  auto nextStep = nextButtons.front();
  
  if (currentCount <= nextStep.prevCounter) {
    std::cout << currentCount << "\n";
    std::cout << nextStep.prevCounter << "\n";
  } else {
    std::cout << currentCount << "\n";
    std::cout << nextStep.prevCounter << "\n";
    std::cout << currentButtons[0];
    throw std::runtime_error("BOOM");
  }
  if (currentCount == nextStep.prevCounter) {
    std::cout << "firing\n";
    std::cout << nextStep.buttons[0];
    currentButtons = nextStep.buttons;
    nextButtons.pop();
    currentCount = 0;
  } else {
    currentCount++;
  }
  return currentButtons;
}

void CompareInterface::loadNextButtons() {
  char c;
  std::string buttons_str;
  while (in.get(c)) {
    buttons_str.push_back(c);
    if (c == BUTTONS_END) {
      break;
    }
  }
  ButtonsStep decoded;
  decoded.prevCounter = DecodeButtonSet(buttons_str, &decoded.buttons[0]);
  std::cout << decoded.buttons[0];
  nextButtons.push(decoded);
}

