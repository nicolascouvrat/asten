#include "screenstream.h"

namespace utils {
ScreenStream::ScreenStream(std::string fileName):
  stream(fileName, std::ios::binary | std::ios::out) {}

void ScreenStream::write(uint8_t palette) {
  stream.write((char*)&palette, sizeof(palette));
}

void ScreenStream::close() {
  stream.write((char*)&closeSignature, sizeof(closeSignature));
  stream.flush();
}
} // namespace utils
