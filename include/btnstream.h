#ifndef GUARD_BTNSTREAM_H
#define GUARD_BTNSTREAM_H

#include <string>
#include <fstream>

namespace utils {
// ButtonsBuffer is the compressed representation of a io_interface::ButtonSet
struct ButtonsBuffer {
  // prevCount is the number of times this buffer "lasts", i.e. it is a
  // convenient way to collapse several ButtonSets into one to save space
  long count;
  // buttons is a bit-array representation of which buttons were in use, in
  // the following order: A, B, SELECT, START, UP, DOWN, LEFT, RIGHT
  uint8_t buttons;
};

struct ResetBuffer {
  bool reset;
  long count;
};

class BtnStream {
  public:
    BtnStream(std::string fileName);
    void write(ButtonsBuffer& buf);
    void write(ResetBuffer& buf);
  private:
    std::ofstream out;
    const char btnSignature[3] = {'B', 'T', 'N'};
    const char rstSignature[3] = {'R', 'S', 'T'};
};
  
} // namespace utils

#endif
