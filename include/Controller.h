#ifndef GUARD_CONTROLLER_H
#define GUARD_CONTROLLER_H

#include <cstdint>
#include <array>
#include "Logger.h"

class Controller {
  public:
    Controller();
    uint8_t read();
    void write(uint8_t);
    enum Buttons: int {
      A,
      B,
      SELECT,
      START,
      UP,
      DOWN,
      LEFT,
      RIGHT,
    };
    void set(std::array<bool, 8>&);
  private:
    Logger log;
    bool buttons[8];
    // index is used to track which button to read next
    // the read order is:
    // A -> B -> select -> start -> up -> down -> left -> right
    uint8_t index, strobe;
};
#endif
