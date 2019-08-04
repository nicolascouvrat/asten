#include "screenstream.h"
#include <iostream>

namespace utils {
ByteAggregator::ByteAggregator(int capacity):
  val(0), count(0), cap(capacity), locked(false) {}

void ByteAggregator::load(uint8_t byte) {
  if (byte > 0b01111111) {
    throw std::runtime_error("writing a value > 127 to byteAggregator");
  }

  if (locked && byte != val) {
    throw std::runtime_error("writing a different value to a locked ByteAggregator");
  }

  if (count == cap) {
    throw std::runtime_error("writing to a full ByteAggregator");
  }

  if (!locked) {
    val = byte;
    locked = true;
  }

  count++;
}

bool ByteAggregator::canLoad(uint8_t byte) {
  if (count == cap) {
    return false;
  }

  if (locked && byte != val) {
    return false;
  }

  return true;
}

std::vector<uint8_t> ByteAggregator::aggregate() {
  std::vector<uint8_t> r;
  // std::cout << (int) val << " count=" << count << "\n";
  uint8_t b = count & 0xff;
  r.push_back(b);
  count >>= 8;

  // we have a bigger count, set val higher bit to 1 to signify "bigger
  // aggregator"
  if (count != 0) {
    uint8_t b = count & 0xff;
    r.push_back(b);
    count >>= 8;
    // Set msb to 1, signifying to the decoded that this value has 2 bytes of
    // count instead of one
    val |= 0x8;
  }

  if (count != 0) {
    // this should not happen
    throw std::runtime_error("aggregated on more than 2 count bytes");
  }
  
  // put it at the beginning, so the decoder knows how many bytes of count it
  // will need to read
  r.insert(r.begin(), val);
  return r;
}

void ByteAggregator::reset() {
  locked = false;
  count = 0;
}

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
    // std::cout << aggregated.size() << "\n";
    stream.write((char*)aggregated.data(), aggregated.size());
    colorAggregator.reset();
  }
  colorAggregator.load(palette);
}

void ScreenStream::close() {
  auto leftover = colorAggregator.aggregate();
  stream.write((char*)leftover.data(), leftover.size());
  stream.write((char*)&SCREENSTREAM_END, sizeof(SCREENSTREAM_END));
  stream.close();
}

uint8_t ScreenStream::read() {
  uint8_t decoded;
  stream.read((char*)&decoded, sizeof(decoded));
  return decoded;
}
} // namespace utils

