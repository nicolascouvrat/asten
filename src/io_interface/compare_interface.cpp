#include "compare_interface.h"


CompareInterface::CompareInterface(InterfaceType t):
  in("game.log")
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
  in.ignore();
  target->colorPixel(x, y, palette);
}

std::array<ButtonSet, 2> CompareInterface::getButtons() {
  char c = in.peek();
  std::array<ButtonSet, 2> buttons({0});
  if (c == BUTTONS_START) {
    std::string buttons_str;
    while (in.get(c)) {
      buttons_str.push_back(c);
      if (c == BUTTONS_END) {
        break;
      }
    }
    auto btn = DecodeButtonSet(buttons_str);
    buttons[0] = btn;
  }
  return buttons;
}

