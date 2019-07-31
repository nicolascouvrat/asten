#include "btnstream.h"
#include <iostream>

namespace utils {
BtnStream::BtnStream(std::string fileName, StreamMode mode) {
  switch (mode) {
    case StreamMode::IN:
      stream.open(fileName, std::ios::binary | std::ios::in);
      break;
    case StreamMode::OUT:
      stream.open(fileName, std::ios::binary | std::ios::out);
      break;
  }
}

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

void BtnStream::close() {
  stream.write((char*)&endSignature, sizeof(endSignature));
  stream.close();
}

void BtnStream::readAll(std::queue<ButtonsBuffer>& buttons, std::queue<ResetBuffer>& resets) {
  while (true) {
    char signature[SIG_SIZE];
    stream.read((char*)&signature, sizeof(signature));
    bool isBtn = true;
    bool isRst = true;
    bool isEnd = true;
    for (int i = 0; i < SIG_SIZE; i++) {
      if (signature[i] != btnSignature[i]) {
        isBtn = false;
      }

      if (signature[i] != rstSignature[i]) {
        isRst = false;
      }

      if (signature[i] != endSignature[i]) {
        isEnd = false;
      }
    }

    if (isEnd) {
      break;
    }

    if (isRst == isBtn) {
      throw std::runtime_error("Could not readAll");
    }

    if (isRst) {
      read(resets);
      continue;
    }

    read(buttons);
  }
}

void BtnStream::read(std::queue<ButtonsBuffer>& buttons) {
  ButtonsBuffer buf;
  stream.read((char*)&buf.count, sizeof(buf.count));
  stream.read((char*)&buf.buttons, sizeof(buf.buttons));
  buttons.push(buf);
}

void BtnStream::read(std::queue<ResetBuffer>& resets) {
  ResetBuffer buf;
  stream.read((char*)&buf.count, sizeof(buf.count));
  stream.read((char*)&buf.reset, sizeof(buf.reset));
  resets.push(buf);
}

} // namespace utils
