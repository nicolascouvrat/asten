#include "screenstream.h"
#include <iostream>

namespace utils {
ScreenStream::ScreenStream(std::string fileName, StreamMode mode, int screenSize):
  colorAggregator(0xffff)
{
  switch (mode) {
    case StreamMode::IN:
      stream.open(fileName, std::ios::binary | std::ios::in);
      break;
    case StreamMode::OUT:
      stream.open(fileName, std::ios::binary | std::ios::out);
      break;
  }
}

void ScreenStream::write(uint8_t palette) {
  if (!colorAggregator.canLoad(palette)) {
    auto aggregated = colorAggregator.aggregate();
    stream << aggregated;
    colorAggregator.reset();
  }
  colorAggregator.load(palette);
}

void ScreenStream::close() {
  auto leftover = colorAggregator.aggregate();
  stream << leftover;
  stream.write((char*)&SCREENSTREAM_END, sizeof(SCREENSTREAM_END));
  stream.close();
}

uint8_t ScreenStream::read() {
  if (currentColor.count != 0) {
    currentColor.count--;
    return currentColor.val;
  }

  if ((uint8_t)stream.peek() == SCREENSTREAM_END) {
    return SCREENSTREAM_END;
  }

  stream >> currentColor;

  // uint8_t byte;
  // stream.read((char*)&byte, sizeof(byte));

  // // XXX: Because we know that we never write anything > 63 (0x3f) to the
  // // ByteAggregator, the only way we get something bigger than 0xbf is when we
  // // stumble upon SCREENSTREAM_END, in which case we can return directly
  // if (byte == SCREENSTREAM_END) {
  //   return SCREENSTREAM_END;
  // }

  // // discard msb
  // currentColor = byte & 0x7f;
  // // decide how to fill count depending on the msb
  // if ((byte >> 7) == 0) {
  //   // in this case, we have only one byte of count, so reuse byte
  //   stream.read((char*)&byte, sizeof(byte));
  //   count = byte;
  // } else {
  //   // in this case, we have only one byte of count, so reuse byte
  //   stream.read((char*)&count, sizeof(count));
  // }

  currentColor.count--;
  return currentColor.val;
}
} // namespace utils

