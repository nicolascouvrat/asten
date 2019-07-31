#ifndef GUARD_BTNSTREAM_H
#define GUARD_BTNSTREAM_H

#include <string>
#include <fstream>
#include <queue>

#include "utilities.h"
#include "streams.h"

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
    BtnStream(std::string fileName, StreamMode mode);
    void write(ButtonsBuffer& buf);
    void write(ResetBuffer& buf);
    void close();
    void readAll(std::queue<ButtonsBuffer>& buttons, std::queue<ResetBuffer>& resets);
    void read(std::queue<ButtonsBuffer>& buttons);
    void read(std::queue<ResetBuffer>& resets);
  private:
    std::fstream stream;
    static const int SIG_SIZE = 3;
    const char btnSignature[SIG_SIZE] = {'B', 'T', 'N'};
    const char rstSignature[SIG_SIZE] = {'R', 'S', 'T'};
    const char endSignature[SIG_SIZE] = {'E', 'N', 'D'};
};

} // namespace utils

#endif
