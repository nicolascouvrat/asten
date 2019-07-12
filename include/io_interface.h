#ifndef GUARD_ENGINE_H
#define GUARD_ENGINE_H

#include <array>
#include <string>

// InterfaceType are the different implementations of IOInterface available
enum InterfaceType {
  // CLASSIC is a Window/Keyboard application
  CLASSIC,
  // SINK is the void implementation (no input, discarted)
  SINK,
  // MONITOR wraps a classic interface and sends usage statistics
  MONITOR,
  // REPLAY wraps a classic interface and uses a file to replay it on screen
  REPLAY,
  // DEBUG wraps a classic interface and uses a file to replay button presses
  // It then compares the output with whatever was in the file
  DEBUG_INTERFACE,
};

// Codes to encode a ButtonSet into a char vector
// All chars corresponding to an ASCII code >= 64 are safe due to how 
// colors can only go from 0 to 63
const char BUTTONS_START = 'X';
const char BUTTONS_END = 'Z';
const char A_CODE = 'A';
const char B_CODE = 'B';
const char SELECT_CODE = 'S';
const char START_CODE = 'T';
const char UP_CODE = 'U';
const char DOWN_CODE = 'D';
const char LEFT_CODE = 'L';
const char RIGHT_CODE = 'R';

// ButtonSet simply is a set of 8 booleans describing which buttons are enabled
// for one controller
struct ButtonSet {
  bool A, B, SELECT, START, UP, DOWN, LEFT, RIGHT;
};

// EncodeButtonSet to a char vector writable to a file
std::string EncodeButtonSet(ButtonSet bs);

// Decode creates a ButtonSet from a properly formatted char array
ButtonSet DecodeButtonSet(std::string in);


// IOInterface describes what any I/O implementation should be able to do in
// order to be usable by the emulator
class IOInterface {
  public:
    // WIDTH and HEIGHT are the native dimensions of the NES output.
    // colorPixel's x should be < WIDTH and y < HEIGHT
    static const int WIDTH = 256;
    static const int HEIGHT = 240;
    static IOInterface* newIOInterface(InterfaceType type);
    // shouldClose returns true if the interface received the instruction to
    // close down
    virtual bool shouldClose() = 0;
    // shouldReset returns true if the interface received the instruction to
    // reset
    virtual bool shouldReset() = 0;
    // render outputs all the pixels to the screen
    virtual void render() = 0;
    // colorPixel sets the color of pixel in position x, y
    // palette should be < 64 (the maximum number of colors supported by a NES)
    virtual void colorPixel(int x, int y, int palette) = 0;
    // getButtons returns which buttons are enabled for each controller
    virtual std::array<ButtonSet, 2> getButtons() = 0;
};

// IOSink is an empty IOInterface implementation that has no imput and ignores
// all output
class IOSink: public IOInterface {
  public:
    bool shouldClose() { return false; };
    bool shouldReset() { return false; };
    void render() {};
    void colorPixel(int, int, int) {};
    std::array<ButtonSet, 2> getButtons() { return std::array<ButtonSet, 2>(); };
};

#endif

