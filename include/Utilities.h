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
    return o << std::hex << std::setw(size) << (long)hi.i << std::dec;
}

template<class T>
inline HexInt<T> hex(T i) { return HexInt<T>(i); } 

struct PPUStateData {
    long clock, frame_count;
    int scan_line, latch_value;
};

inline std::istream& operator>> (std::istream& in, PPUStateData& d) {
    in >> d.clock;
    in >> d.frame_count;
    in >> d.scan_line;
    in >> d.latch_value;
    return in;
}

inline std::ostream& operator<< (std::ostream& o, const PPUStateData& d) {
    return o
        << std::left
        << d.frame_count << " "
        << "LV: " << hex((uint8_t)d.latch_value)
        << "SL: " << d.scan_line << " "
        << "CYC: " << d.clock;
}

inline bool operator== (const PPUStateData& a, const PPUStateData& b) {
    return a.clock == b.clock
        && a.frame_count == b.frame_count
        && a.scan_line == b.scan_line
        && a.latch_value == b.latch_value;
}

struct CPUStateData {
    /* Used to view the inside of the CPU */
    int A, X, Y, sp, pc, flags;
    int latest_instruction, cycle;
};

// instruction names
static std::string instruction_names[256] = {
    "BRK", "ORA", "KIL", "SLO", "NOP", "ORA", "ASL", "SLO",
    "PHP", "ORA", "ASL", "ANC", "NOP", "ORA", "ASL", "SLO",
    "BPL", "ORA", "KIL", "SLO", "NOP", "ORA", "ASL", "SLO",
    "CLC", "ORA", "NOP", "SLO", "NOP", "ORA", "ASL", "SLO",
    "JSR", "AND", "KIL", "RLA", "BIT", "AND", "ROL", "RLA",
    "PLP", "AND", "ROL", "ANC", "BIT", "AND", "ROL", "RLA",
    "BMI", "AND", "KIL", "RLA", "NOP", "AND", "ROL", "RLA",
    "SEC", "AND", "NOP", "RLA", "NOP", "AND", "ROL", "RLA",
    "RTI", "EOR", "KIL", "SRE", "NOP", "EOR", "LSR", "SRE",
    "PHA", "EOR", "LSR", "ALR", "JMP", "EOR", "LSR", "SRE",
    "BVC", "EOR", "KIL", "SRE", "NOP", "EOR", "LSR", "SRE",
    "CLI", "EOR", "NOP", "SRE", "NOP", "EOR", "LSR", "SRE",
    "RTS", "ADC", "KIL", "RRA", "NOP", "ADC", "ROR", "RRA",
    "PLA", "ADC", "ROR", "ARR", "JMP", "ADC", "ROR", "RRA",
    "BVS", "ADC", "KIL", "RRA", "NOP", "ADC", "ROR", "RRA",
    "SEI", "ADC", "NOP", "RRA", "NOP", "ADC", "ROR", "RRA",
    "NOP", "STA", "NOP", "SAX", "STY", "STA", "STX", "SAX",
    "DEY", "NOP", "TXA", "XAA", "STY", "STA", "STX", "SAX",
    "BCC", "STA", "KIL", "AHX", "STY", "STA", "STX", "SAX",
    "TYA", "STA", "TXS", "TAS", "SHY", "STA", "SHX", "AHX",
    "LDY", "LDA", "LDX", "LAX", "LDY", "LDA", "LDX", "LAX",
    "TAY", "LDA", "TAX", "LAX", "LDY", "LDA", "LDX", "LAX",
    "BCS", "LDA", "KIL", "LAX", "LDY", "LDA", "LDX", "LAX",
    "CLV", "LDA", "TSX", "LAS", "LDY", "LDA", "LDX", "LAX",
    "CPY", "CMP", "NOP", "DCP", "CPY", "CMP", "DEC", "DCP",
    "INY", "CMP", "DEX", "AXS", "CPY", "CMP", "DEC", "DCP",
    "BNE", "CMP", "KIL", "DCP", "NOP", "CMP", "DEC", "DCP",
    "CLD", "CMP", "NOP", "DCP", "NOP", "CMP", "DEC", "DCP",
    "CPX", "SBC", "NOP", "ISB", "CPX", "SBC", "INC", "ISB",
    "INX", "SBC", "NOP", "SBC", "CPX", "SBC", "INC", "ISB",
    "BEQ", "SBC", "KIL", "ISB", "NOP", "SBC", "INC", "ISB",
    "SED", "SBC", "NOP", "ISB", "NOP", "SBC", "INC", "ISB",
};

inline std::istream& operator>> (std::istream& in, CPUStateData& d) {
    in >> d.pc;
    in >> d.A;
    in >> d.X;
    in >> d.Y;
    in >> d.flags;
    in >> d.sp;
    in >> std::dec >> d.cycle;
    return in;
}

inline std::ostream& operator<< (std::ostream& o, const CPUStateData& d) {
    return o
        << std::left
        << hex((uint16_t)d.pc)
        << "PREV_OP: " << instruction_names[d.latest_instruction] << " "
        << "A: " << hex((uint8_t)d.A)
        << "X: " << hex((uint8_t)d.X)
        << "Y: " << hex((uint8_t)d.Y)
        << "P: " << hex((uint8_t)d.flags)
        << "SP: " << hex((uint8_t)d.sp)
        << "CYC: " << hex((uint8_t)d.cycle);
}

inline bool operator== (const CPUStateData& a, const CPUStateData& b) {
    return a.A == b.A
        && a.X == b.X
        && a.Y == b.Y
        && a.sp == b.sp
        && a.pc == b.pc
        && a.flags == b.flags
        && a.cycle == b.cycle;
}

struct NESCartridgeData {
    int prg_rom_unit, chr_rom_unit;
    int mirror_id, mapper_id;
};

#endif

