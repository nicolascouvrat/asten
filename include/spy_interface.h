#ifndef GUARD_SPY_INTERFACE_H
#define GUARD_SPY_INTERFACE_H

#include <array>
#include <string>
#include <fstream>

#include "io_interface.h"
#include "btnstream.h"
#include "screenstream.h"

// SpyInterface is a wrapper around another IOInterface.
//
// It will mimic its properties all the while sending usage statistics
class SpyInterface: public IOInterface {
  public:
    SpyInterface(InterfaceType targetType);
    bool shouldClose();
    bool shouldReset();
    void render();
    void colorPixel(int x, int y, int palette);
    std::array<ButtonSet, 2> getButtons();
  private:
    static const int BUF_SIZE = 1048576; // 1 MB
    IOInterface *target;

    utils::ScreenStream screenStream;
    utils::BtnStream btnStream;

    // identicalCount indicates the number of times getButtons() has been called
    // before there was a change in currentButtons;
    long identicalCount;
    std::array<ButtonSet, 2> currentButtons;

    long identicalRstCount;
    bool currentReset;
};

#endif
