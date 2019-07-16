#include "compare_interface.h"
#include <iostream>


CompareInterface::CompareInterface(InterfaceType t):
  screenIn("screen.log"),
  buttonsIn("buttons.log"),
  currentButtons({0})
{
  target = IOInterface::newIOInterface(t);
  char c = buttonsIn.peek();
  while (c != EOF) {
    loadNextButtons();
    c = buttonsIn.peek();
  }
}

bool CompareInterface::shouldClose() { return isDone; }

bool CompareInterface::shouldReset() { return target->shouldReset(); }

void CompareInterface::render() {
  target->render();
}

void CompareInterface::colorPixel(int x, int y, int palette) {
  char c;
  screenIn.get(c);
  if (c == 'Y') {
    isDone = true;
    return;
  }
  if ((int)c != palette) {
    // TODO: this probably does not work for games where random numbers are
    // involved...
    // throw std::runtime_error("OOPS");
  }
  target->colorPixel(x, y, palette);
}

std::array<ButtonSet, 2> CompareInterface::getButtons() {
  if (nextButtons.size() > 0) {
    auto next = nextButtons.front();

    if (currentCount == next.prevCounter) {
      currentButtons = next.buttons;
      nextButtons.pop();
      currentCount = 0;
      return currentButtons;
    }
  }

  currentCount ++;
  return currentButtons;
}

void CompareInterface::loadNextButtons() {
  char c;
  std::string buttons_str;
  while (buttonsIn.get(c)) {
    buttons_str.push_back(c);
    if (c == BUTTONS_END) {
      break;
    }
  }
  ButtonsStep decoded;
  decoded.prevCounter = DecodeButtonSet(buttons_str, &decoded.buttons[0]);
  nextButtons.push(decoded);
}

