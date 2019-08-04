#ifndef GUARD_SCREENSTREAM_H
#define GUARD_SCREENSTREAM_H

#include <string>
#include <fstream>
#include <vector>

#include "streams.h"

namespace utils {
// SCREENSTREAM_END is the value found at the end of files produced by
// ScreenStream (an EOF of sorts)
const uint8_t SCREENSTREAM_END = 255;

// ByteAggregator will aggregate indentical values. Reusable.
class ByteAggregator {
  public:
    // load a byte into the aggregator
    void load(uint8_t byte);
    // canLoad returns true if additional byte can be aggregated (equal to the
    // internal value, and capacity not exceeded)
    bool canLoad(uint8_t byte);
    // aggregate everything that was loaded into the aggregator
    std::vector<uint8_t> aggregate();
    // reset the aggregator 
    void reset();
    // capacity is the number of bytes the aggregator can take before needing to reset
    ByteAggregator(int capacity);
  private:
    uint8_t val;
    int count;
    int cap;
    bool locked;
};

class ScreenStream {
  public:
    // XXX: screenSize is supposed to be a multiple of 8
    ScreenStream(std::string fileName, StreamMode mode, int screenSize);
    void write(uint8_t palette);
    void close();
    // read returns the next byte as an int
    uint8_t read();
  private:
    std::fstream stream;
    // diffs is of length screenSize/8, where each bit represents one pixel with
    // a value of 0 if its color value stayed the same, 1 if it changed
    std::vector<uint8_t> diffs;
    // colors is of length screenSize, each byte being the current color of a
    // pixel.
    std::vector<uint8_t> colors;
    // it points to where we currently are in colors and diffs
    int it;

    // colorDiffs stores the colors of the pixels which color actually changed
    std::vector<uint8_t> colorDiffs;

    ByteAggregator diffAggregator;
    ByteAggregator colorAggregator;
};
} // namespace utils

#endif
