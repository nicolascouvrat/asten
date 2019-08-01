#ifndef GUARD_REPLAY_INTERFACE_H
#define GUARD_REPLAY_INTERFACE_H

#include <array>
#include <fstream>

#include "io_interface.h"
#include "screenstream.h"

// ReplayInterface is a wrapper around another interface
//
// It will read from a saved monitor file and display whatever was saved there
class ReplayInterface: public IOInterface {
  public:
    ReplayInterface(InterfaceType targetType);
    bool shouldClose();
    bool shouldReset();
    void render();
    void colorPixel(int x, int y, int palette);
    std::array<ButtonSet, 2> getButtons();
  private:
    IOInterface *target;
    utils::ScreenStream screenStream;

    bool isClose;
};

#endif
