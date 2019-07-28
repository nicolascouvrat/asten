#include "screenstream.h"

namespace utils {
ScreenStream::ScreenStream(std::string fileName):
  stream(fileName, std::ios::binary | std::ios::out | std::ios::in) {}

void ScreenStream::write(uint8_t palette) {
  stream.write((char*)&palette, sizeof(palette));
}

void ScreenStream::close() {
  stream.write((char*)&closeSignature, sizeof(closeSignature));
  stream.close();
}

bool ScreenStream::isClose() {
  int start = stream.tellg();
  for (int i = 0; i < sizeof(closeSignature); i++) {
    stream.seekg(start + i);

    if (stream.peek() != closeSignature[i]) {
      stream.seekg(start);
      return false;
    }
  }
  return true;
}

uint8_t ScreenStream::read() {
  uint8_t decoded;
  stream.read((char*)&decoded, sizeof(decoded));
  return decoded;
}
} // namespace utils
