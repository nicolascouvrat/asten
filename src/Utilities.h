#ifndef GUARD_UTILITIES_H
#define GUARD_UTILITIES_H

#include <iostream>
#include <cstdint>

struct HexChar {
    uint8_t c;
    HexChar(uint8_t _c): c(_c) {};
};

inline std::ostream& operator<< (std::ostream& o, const HexChar &hc) {
    return o << std::hex << (int)hc.c;
    // TODO: reset stream to previous state
}

inline HexChar hex(uint8_t c) { return HexChar(c); } 

#endif

