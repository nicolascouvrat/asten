#ifndef GUARD_SCREENSTREAM_H
#define GUARD_SCREENSTREAM_H

#include <string>
#include <fstream>

namespace utils {
class ScreenStream {
  public:
    ScreenStream(std::string fileName);
    void write(uint8_t palette);
    void close();
  private:
    std::fstream stream;
    const char closeSignature[3] = {'E', 'N', 'D'};
};
} // namespace utils

#endif
