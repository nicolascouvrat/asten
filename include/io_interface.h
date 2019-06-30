#ifndef GUARD_ENGINE_H
#define GUARD_ENGINE_H

#include <array>

class IOInterface {
  public:
    static IOInterface* newIOInterface();
    virtual bool isRunning() = 0;
    virtual void render() = 0;
    virtual void colorPixel(int x, int y, int palette) = 0;
    virtual std::array<bool, 8> getButtons() = 0;
    virtual bool shouldReset() = 0;
};

#endif

