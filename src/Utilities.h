#ifndef GUARD_UTILITIES_H
#define GUARD_UTILITIES_H

#include <iostream>
#include <cstdint>
#include <iomanip>

template<class T>
struct HexInt {
    /* Handles the output of different int types as hex
     * Intended to be use with int types only.
     * */
    T i;
    HexInt(T _i): i(_i) {};
};

template<class T>
inline std::ostream& operator<< (std::ostream& o, const HexInt<T> &hi) {
    int size = sizeof(hi.i) * 2 + 2; // number of digits plus two spaces
    return o << std::hex << std::setw(size) << (int)hi.i << std::dec;
}

template<class T>
inline HexInt<T> hex(T i) { return HexInt<T>(i); } 

#endif

