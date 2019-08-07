#ifndef BYTE_AGGREGATOR_H
#define BYTE_AGGREGATOR_H

#include <vector>
#include <stdint.h>
#include <stdexcept>
#include <iostream>

namespace utils {
// AggrBytes is a compressed representation of a series of identical bytes.
// 
// It is encoded over 2 bytes if is8, else 3.
struct AggrBytes {
  // val is the value of the repeated byte, necessarily < 0x80
  uint8_t val;
  // count is the number of times val is repeated
  uint16_t count;
  // is8 indicates whether count can hold on only one byte instead of 2
  bool is8;

  AggrBytes(): val(0), count(0), is8(true) {}
};

// Read and write AggrBytes
std::ostream& operator<< (std::ostream& out, AggrBytes& ab);
std::istream& operator>> (std::istream& in, AggrBytes& ab);

// ByteAggregator will aggregate indentical bytes into an AggrBytes. Reusable.
class ByteAggregator {
  public:
    // MAX is the maximum tolerated value for load()
    static const int MAX = 0x7f;
    // CAP is the number of bytes the aggregator can take before needing to reset
    static const int CAP = 0xffff;
    // load a byte into the aggregator. Byte has to be <= MAX
    void load(uint8_t byte);
    // canLoad returns true if additional byte can be aggregated (equal to the
    // internal value, and capacity not exceeded)
    bool canLoad(uint8_t byte);
    // aggregate everything that was loaded into the aggregator
    AggrBytes aggregate();
    // reset the aggregator 
    void reset();

    // capacity is the number of bytes the aggregator can take before needing to reset
    ByteAggregator(int capacity);
  private:
    AggrBytes aggr;
    bool locked;
};
}
#endif
