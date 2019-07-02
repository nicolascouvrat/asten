#ifndef GUARD_CONTROLLER_H
#define GUARD_CONTROLLER_H

#include <cstdint>
#include <array>

#include "logger.h"
#include "io_interface.h"

// Buttons are the different possible buttons supported by the NES
// The order is important, as the enum value is equal to the index in the button
// byte stored in the controller
enum Buttons {
  A,
  B,
  SELECT,
  START,
  UP,
  DOWN,
  LEFT,
  RIGHT,
  // the total number of buttons
  Buttons_MAX = RIGHT,
};

// Controller describes a game controller seen by the cpu, i.e. independent of
// its hardware
class Controller {
  public:
    Controller();
    // read returns which buttons are set
    uint8_t read();
    // write the strobe value in the controller
    void write(uint8_t strobe);
    // sets the internal register according to the buttons activated from the
    // interface
    void set(ButtonSet);
  private:
    Logger log;
    bool buttons[8];
    // index is used to track which button to read next
    // the read order is:
    // A -> B -> select -> start -> up -> down -> left -> right
    uint8_t index, strobe;
};
#endif
