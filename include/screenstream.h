#ifndef GUARD_SCREENSTREAM_H
#define GUARD_SCREENSTREAM_H

#include <string>
#include <fstream>

namespace utils {
// SCREENSTREAM_END is the value found at the end of files produced by
// ScreenStream (an EOF of sorts)
const uint8_t SCREENSTREAM_END = 255;

class ScreenStream {
  public:
    ScreenStream(std::string fileName);
    void write(uint8_t palette);
    void close();
    // read returns the next byte as an int
    uint8_t read();
  private:
    std::fstream stream;
};
} // namespace utils

#endif
