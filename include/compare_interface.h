#ifndef GUARD_COMPARE_INTERFACE_H
#define GUARD_COMPARE_INTERFACE_H

#include <array>
#include <fstream>
#include <queue>

#include "io_interface.h"

struct ButtonsStep {
  long prevCounter;
  std::array<ButtonSet, 2> buttons;
};

inline std::ostream& operator<< (std::ostream& o, const ButtonSet& bs) {
  return o
    << "A:" << bs.A << " "
    << "B:" << bs.B << " "
    << "SELECT:" << bs.SELECT << " "
    << "START:" << bs.START << " "
    << "UP:" << bs.UP << " "
    << "DOWN:" << bs.DOWN << " "
    << "LEFT:" << bs.LEFT << " "
    << "RIGHT:" << bs.RIGHT << "\n";
}

// CompareInterface is a wrapper around another interface
//
// It will read from a saved monitor file, replay its button presses and compare
// output
class CompareInterface: public IOInterface {
  public:
    CompareInterface(InterfaceType targetType);
    bool shouldClose();
    bool shouldReset();
    void render();
    void colorPixel(int x, int y, int palette);
    std::array<ButtonSet, 2> getButtons();
  private:
    IOInterface *target;

    std::ifstream screenIn;
    std::ifstream buttonsIn;
  
    std::array<ButtonSet, 2> currentButtons;
    std::queue<ButtonsStep> nextButtons;
    long currentCount;
    void loadNextButtons();
    bool isDone;
};

#endif

