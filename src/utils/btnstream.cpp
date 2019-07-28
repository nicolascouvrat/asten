#include "btnstream.h"

namespace utils {
BtnStream::BtnStream(std::string fileName):
  out(fileName, std::ios::binary) {}

void BtnStream::write(ButtonsBuffer& buf) {
  // write signature
  out.write((char*)&btnSignature, sizeof(btnSignature));
  out.write((char*)&buf.count, sizeof(buf.count));
  out.write((char*)&buf.buttons, sizeof(buf.buttons));
}

void BtnStream::write(ResetBuffer& buf) {
  out.write((char*)&rstSignature, sizeof(rstSignature));
  out.write((char*)&buf.count, sizeof(buf.count));
  out.write((char*)&buf.reset, sizeof(buf.reset));
}

} // namespace utils
