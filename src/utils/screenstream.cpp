#include "screenstream.h"
#include <iostream>

namespace utils {
ScreenStream::ScreenStream(std::string fileName, StreamMode mode, int screenSize):
  diffs(screenSize/8), colors(screenSize, 255), it(0)
{
  switch (mode) {
    case StreamMode::IN:
      stream.open(fileName, std::ios::binary | std::ios::in);
      break;
    case StreamMode::OUT:
      stream.open(fileName, std::ios::binary | std::ios::out);
      break;
  }
  // We know the maximal size of all vectors, so reserve it from the start to
  // avoid useless relocations
  colorDiffs.reserve(screenSize);
}

void ScreenStream::write(uint8_t palette) {
  // for diffs, it points to a bit, not to a byte
  int diffIndex = it / 8;
  // it goes from left to right, but inside one diff the offset goes right to
  // left
  int diffOffset = 7 - it % 8;

  // update diffs array
  uint8_t prevColor = colors[it];
  uint8_t currDiff = diffs[diffIndex];
  if (palette != prevColor) {
    // 0 everywhere except where the diff is, then bitwise OR to set it while
    // preserving the rest
    uint8_t filter = 1 << diffOffset; 
    diffs[diffIndex] = currDiff | filter;
    // add palette to color buffer
    colorDiffs.push_back(palette);
  } else {
    // 1 everywhere except where the diff is, then bitwise AND to set it while
    // preserving the rest
    uint8_t filter = ~(1 << diffOffset);
    diffs[diffIndex] = currDiff & filter; 
  }

  // update colors array
  colors[it] = palette;
  // go to next pixel
  it++;

  if (diffOffset == 7) {
    // we just finished writing one full byte, send it to the file
    stream.write((char*)&diffs[diffIndex], sizeof(uint8_t));
  }

  if (it == (colors.size() - 1)) {
    // we're done with one full screen, write additional info'
    uint16_t colorDiffsSize = colorDiffs.size();
    std::cout << "writing colorDiffsSize=" << colorDiffs.size() << "\n";
    stream.write((char*)&colorDiffsSize, sizeof(uint8_t));
    stream.write((char*)colorDiffs.data(), colorDiffs.size());

    // finally, resize colorDiffs for another loop
    colorDiffs.resize(0);
    it = 0;
  }
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
