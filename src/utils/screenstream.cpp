#include "screenstream.h"
#include <iostream>

namespace utils {
ScreenStream::ScreenStream(std::string fileName):
  stream(fileName, std::ios::binary | std::ios::out | std::ios::in) {}

void ScreenStream::write(uint8_t palette) {
  stream.write((char*)&palette, sizeof(palette));
}

void ScreenStream::close() {
  stream.write((char*)&SCREENSTREAM_END, sizeof(SCREENSTREAM_END));
  stream.close();
}

uint8_t ScreenStream::read() {
  uint8_t decoded;
  stream.read((char*)&decoded, sizeof(decoded));
  return decoded;
}
} // namespace utils
