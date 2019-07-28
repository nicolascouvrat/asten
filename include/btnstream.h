#ifndef GUARD_BTNSTREAM_H
#define GUARD_BTNSTREAM_H

#include <string>
#include <fstream>

#include "utilities.h"

namespace utils {
// ButtonsBuffer is the compressed representation of a io_interface::ButtonSet
struct ButtonsBuffer {
  // count is the number of times this buffer "lasts", i.e. it is a
  // convenient way to collapse several ButtonSets into one to save space
  long count;
  // buttons is a bit-array representation of which buttons were in use, in
  // the following order: A, B, SELECT, START, UP, DOWN, LEFT, RIGHT
  uint8_t buttons;
};

inline std::ostream& operator<< (std::ostream& o, const ButtonsBuffer& buf) {
  return o << "buttons: " << hex(buf.buttons) << " count: " << buf.count << "\n";
}

// ResetBuffer is the compressed representation of the usage of the reset button
struct ResetBuffer {
  // count is the number of times this buffer "lasts", i.e. it is a
  // convenient way to collapse several resets into one to save space
  long count;
  bool reset;
};

inline std::ostream& operator<< (std::ostream& o, const ResetBuffer& buf) {
  return o << "reset: " << buf.reset << " count: " << buf.count << "\n";
}

// BtnStream is a wrapper around std::fstream that allows for the writing and reading
// of ButtonsBuffer and ResetBuffer
class BtnStream {
  public:
    BtnStream(std::string fileName);
    void write(ButtonsBuffer& buf);
    void write(ResetBuffer& buf);
    void close();
  private:
    std::fstream stream;
    const char btnSignature[3] = {'B', 'T', 'N'};
    const char rstSignature[3] = {'R', 'S', 'T'};
};

} // namespace utils

#endif
