#ifndef GUARD_UTILITIES_H
#define GUARD_UTILITIES_H

#include <iostream>

struct HexChar {
    unsigned char c;
    HexChar(unsigned char _c): c(_c) {};
};

inline std::ostream& operator<< (std::ostream& o, const HexChar &hc) {
    return o << std::hex << (int)hc.c;
    // TODO: reset stream to previous state
}

inline HexChar hex(unsigned char c) { return HexChar(c); } 

#endif

