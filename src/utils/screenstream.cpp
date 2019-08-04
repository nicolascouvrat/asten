#include "screenstream.h"
#include <iostream>

namespace utils {
ByteAggregator::ByteAggregator(int capacity):
  val(0), count(0), cap(capacity), locked(false) {}

void ByteAggregator::load(uint8_t byte) {}

bool ByteAggregator::canLoad(uint8_t byte) { return false; }

std::vector<uint8_t> ByteAggregator::aggregate() {
  std::vector<uint8_t> r;
  return r;
}

void ByteAggregator::reset() {}

ScreenStream::ScreenStream(std::string fileName, StreamMode mode, int screenSize):
  diffs(screenSize/8), colors(screenSize, 255), it(0),
  diffAggregator(0xffff), colorAggregator(0xffff)
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
    // if the aggregator is full (meaning we changed value), then aggregate and
    // write, otherwise load the value in the aggregator and move on
    if (!diffAggregator.canLoad(diffs[diffIndex])) {
      auto aggregated = diffAggregator.aggregate();
      stream.write((char*)aggregated.data(), aggregated.size());
      diffAggregator.reset();
    }
    diffAggregator.load(diffs[diffIndex]);
  }

  if (it == (colors.size() - 1)) {
    // we're done with one full screen, write additional info
    // first, write any diff leftovers
    auto aggregatedDiffs = diffAggregator.aggregate();
    stream.write((char*)aggregatedDiffs.data(), aggregatedDiffs.size());

    // aggregate colorDiffs
    std::vector<uint8_t> compressed, aggregated;
    for (auto it = colorDiffs.begin(); it != colorDiffs.end(); it ++) {
      if (!colorAggregator.canLoad(*it)) {
        aggregated = colorAggregator.aggregate();
        compressed.insert(compressed.end(), aggregated.begin(), aggregated.end());
        colorAggregator.reset();
      }
      colorAggregator.load(*it);
    }

    // aggregate the leftovers as well
    aggregated = colorAggregator.aggregate();
    compressed.insert(compressed.end(), aggregated.begin(), aggregated.end());

    // compressed should not have more than 2 * 240 * 256 bytes
    int compressedSize = compressed.size();
    std::cout << "writing colorDiffsSize=" << compressed.size() << "\n";
    stream.write((char*)&compressedSize, sizeof(int));
    stream.write((char*)compressed.data(), compressed.size());

    // finally, resize colorDiffs for another loop
    diffAggregator.reset();
    colorAggregator.reset();
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

