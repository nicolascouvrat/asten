#ifndef GUARD_COMPARE_INTERFACE_H
#define GUARD_COMPARE_INTERFACE_H

#include <array>
#include <fstream>
#include <queue>

#include "io_interface.h"
#include "btnstream.h"
#include "screenstream.h"


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

    utils::BtnStream btnStream;
    utils::ScreenStream screenStream;
  
    std::queue<utils::ButtonsBuffer> nextButtons;
    std::queue<utils::ResetBuffer> nextResets;

    long remainingCount;
    std::array<ButtonSet, 2> currentButtons;
    void loadNextButtons();

    long remainingRstCount;
    bool currentReset;
    void loadNextReset();

    bool isDone;
};

#endif

