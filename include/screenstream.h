#ifndef GUARD_SCREENSTREAM_H
#define GUARD_SCREENSTREAM_H

#include <string>
#include <fstream>
#include <vector>

#include "streams.h"
#include "byte_aggregator.h"

namespace utils {
// SCREENSTREAM_END is the value found at the end of files produced by
// ScreenStream (an EOF of sorts)
const uint8_t SCREENSTREAM_END = 255;

// ScreenStream is a wrapper around a bit stream that writes palette palette
// values to a file, compressing them in the process. Conversly, it can also
// decompress it to return the values in the same order.
class ScreenStream {
  public:
    // XXX: screenSize is supposed to be a multiple of 8
    ScreenStream(std::string fileName, StreamMode mode, int screenSize);
    // write palette to the file, assuming palette is < 64
    void write(uint8_t palette);
    // read the palette value. The nth call to read() is guaranteed to return
    // the same value that was written with the nth call to write().
    uint8_t read();
    // close the underlying stream. As output is bufferized, this must be called
    // in order to avoid missing data
    void close();
  private:
    std::fstream stream;

    AggrBytes currentColor;
    ByteAggregator colorAggregator;
};
} // namespace utils

#endif
