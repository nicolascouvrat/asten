#ifndef GUARD_ENGINE_H
#define GUARD_ENGINE_H

#include <array>

// ButtonSet simply is a set of 8 booleans describing which buttons are enabled
// for one controller
struct ButtonSet {
  bool A, B, SELECT, START, UP, DOWN, LEFT, RIGHT;
};

// IOInterface describes what any I/O implementation should be able to do in
// order to be usable by the emulator
class IOInterface {
  public:
    static IOInterface* newIOInterface();
    // shouldClose returns true if the interface received the instruction to
    // close down
    virtual bool shouldClose() = 0;
    // shouldReset returns true if the interface received the instruction to
    // reset
    virtual bool shouldReset() = 0;
    // render outputs all the pixels to the screen
    virtual void render() = 0;
    // colorPixel sets the color of pixel in position x, y
    virtual void colorPixel(int x, int y, int palette) = 0;
    // getButtons returns which buttons are enabled for each controller
    virtual std::array<ButtonSet, 2> getButtons() = 0;
};

#endif

