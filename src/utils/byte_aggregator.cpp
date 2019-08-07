#include "byte_aggregator.h"

namespace utils {
std::ostream& operator<< (std::ostream& out, AggrBytes& ab) {
  if (ab.is8) {
    out.write((char*)&ab.val, sizeof(uint8_t));
    uint8_t smallCount = ab.count & 0xff;
    out.write((char*)&smallCount, sizeof(uint8_t));
    return out;
  }

  // signify that count is encoded over two bytes by setting the msb of value
  uint8_t outVal = ab.val | 0x80;
  out.write((char*)&outVal, sizeof(uint8_t));
  out.write((char*)&ab.count, sizeof(uint16_t));

  return out;
}

std::istream& operator>> (std::istream& in, AggrBytes& ab) {
  uint8_t inVal;
  in.read((char*)&inVal, sizeof(uint8_t));

  // Discard msb
  ab.val = inVal & 0x7f;

  if (inVal >> 7 == 0) {
    ab.is8 = true;
    in.read((char*)&ab.count, sizeof(uint8_t)); 
    return in;
  }

  ab.is8 = false;
  in.read((char*)&ab.count, sizeof(uint16_t));
  return in;
}

ByteAggregator::ByteAggregator(int capacity): locked(false) {}

void ByteAggregator::load(uint8_t byte) {
  if (byte >= ByteAggregator::MAX) {
    throw std::runtime_error("writing a value too big to byteAggregator");
  }

  if (locked && byte != aggr.val) {
    throw std::runtime_error("writing a different value to a locked ByteAggregator");
  }

  if (aggr.count == ByteAggregator::CAP) {
    throw std::runtime_error("writing to a full ByteAggregator");
  }

  if (!locked) {
    aggr.val = byte;
    locked = true;
  }

  if (aggr.count == 0xff) {
    aggr.is8 = false;
  }

  aggr.count++;
}

bool ByteAggregator::canLoad(uint8_t byte) {
  if (aggr.count == ByteAggregator::CAP) {
    return false;
  }

  if (locked && byte != aggr.val) {
    return false;
  }

  return true;
}

AggrBytes ByteAggregator::aggregate() {
  return aggr;
}
//   // std::cout << (int) val << " count=" << count << "\n";
// 
//   // TODO: this should probably use the write() function to ensure endianess is
//   // correct? Right now we write in little endian so if the platform is little
//   // endian then it can decode, else no
//   uint8_t b = count & 0xff;
//   r.push_back(b);
//   count >>= 8;
// 
//   // we have a bigger count, set val higher bit to 1 to signify "bigger
//   // aggregator"
//   if (count != 0) {
//     uint8_t b = count & 0xff;
//     r.push_back(b);
//     count >>= 8;
//     // Set msb to 1, signifying to the decoded that this value has 2 bytes of
//     // count instead of one
//     val |= 0x80;
//   }
// 
//   if (count != 0) {
//     // this should not happen
//     throw std::runtime_error("aggregated on more than 2 count bytes");
//   }
//   
//   // put it at the beginning, so the decoder knows how many bytes of count it
//   // will need to read
//   r.insert(r.begin(), val);
//   return r;
// }

void ByteAggregator::reset() {
  locked = false;
  aggr = AggrBytes();
}
} // namespace utils

