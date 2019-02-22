#include "Controller.h"
// TODO: remove
#include "Utilities.h"

Controller::Controller(): 
  buttons{0}, log(Logger::get_logger("Controller"))
{
  log.set_level(DEBUG);
  strobe = 0;
  index = 0;
}

// If the strobe is set, return the value of A
// Else, roll through the different buttons in order
uint8_t Controller::read() {
  if (strobe == 1) {
    index = 0;
    return buttons[0];
  }
  uint8_t value = buttons[index];
  index++;
  if (index > 7)
    index = 0;
  return value; 
}

void Controller::write(uint8_t value) {
  strobe = value & 1;
}

void Controller::set(std::array<bool, 8>& values) {
  for (int i = 0; i < 8; i++) {
    buttons[i] = values[i];
  }
}
