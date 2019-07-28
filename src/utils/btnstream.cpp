#include "btnstream.h"
#include <iostream>

namespace utils {
BtnStream::BtnStream(std::string fileName):
  stream(fileName, std::ios::binary | std::ios::out) {}

void BtnStream::write(ButtonsBuffer& buf) {
  stream.write((char*)&btnSignature, sizeof(btnSignature));
  stream.write((char*)&buf.count, sizeof(buf.count));
  stream.write((char*)&buf.buttons, sizeof(buf.buttons));
}

void BtnStream::write(ResetBuffer& buf) {
  stream.write((char*)&rstSignature, sizeof(rstSignature));
  stream.write((char*)&buf.count, sizeof(buf.count));
  stream.write((char*)&buf.reset, sizeof(buf.reset));
}

void BtnStream::flush() {
  stream.flush();
}

} // namespace utils
