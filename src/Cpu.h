#ifndef GUARD_CPU_H
#define GUARD_CPU_H

#include "Memory.h"
#include "Logger.h"
#include "Utilities.h"
#include <iostream>
#include <cstdint>
#include <iomanip>
#include <bitset>


class CPU {
public:
    CPU();
    CPUMemory& get_memory();
    void step();
    void reset();
    // TODO: remove
    void execute(uint8_t);
    void debug_dump();
    friend CPUStateData& operator<< (CPUStateData& d, const CPU& c) {
        d.A = c.A;
        d.X = c.X;
        d.Y = c.Y;
        d.sp = c.sp;
        d.pc = c.pc;
        d.flags = c.get_flags();
        d.latest_instruction = c.latest_instruction;
        return d;
    };
private:
    CPUMemory mem;
    uint8_t A, X, Y;                // registers
    uint8_t sp;                     // stack pointer
    uint16_t pc;                    // program counter
    bool C, Z, I, D, B, U, O, N;    // processor flags
    long clock;                     // internal CPU clock (number of cycles)
    // debug
    uint8_t latest_instruction;
    enum InterruptType: uint8_t {
        NMI,
        RESET,
        IRQ,
        BRK
    };
    // addressing modes (1 to 13)
    enum AddressingMode: uint16_t {
        _, // unused
        ABSOLUTE_MODE,
        ABSOLUTEX_MODE,
        ABSOLUTEY_MODE,
        ACCUMULATOR_MODE,
        IMMEDIATE_MODE,
        IMPLIED_MODE,
        INDEXED_INDIRECT_MODE,
        INDIRECT_MODE,
        INDIRECT_INDEXED_MODE,
        RELATIVE_MODE,
        ZERO_PAGE_MODE,
        ZERO_PAGEX_MODE,
        ZERO_PAGEY_MODE
    };
    // info for instructions
    struct InstructionInfo {
        uint16_t pc;
        uint16_t address;
        AddressingMode mode;
    };
    // instruction table
    typedef void (CPU::*cpu_instruction)(const InstructionInfo&);
    cpu_instruction instruction_table[256];
    // addressing mode for each of the 256 instructions
    static constexpr uint8_t instruction_modes[256] = {
        6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
        10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
        1, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
        10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
        6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
        10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
        6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 8, 1, 1, 1,
        10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
        5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
        10, 9, 6, 9, 12, 12, 13, 13, 6, 3, 6, 3, 2, 2, 3, 3,
        5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
        10, 9, 6, 9, 12, 12, 13, 13, 6, 3, 6, 3, 2, 2, 3, 3,
        5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
        10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
        5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
        10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
    };
    // number of bytes to look up for each instructions
    static constexpr uint8_t instruction_sizes[256] = {
        1, 2, 0, 2, 2, 2, 2, 2, 1, 2, 1, 0, 3, 3, 3, 3,
        2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
        3, 2, 0, 2, 2, 2, 2, 2, 1, 2, 1, 0, 3, 3, 3, 3,
        2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
        1, 2, 0, 2, 2, 2, 2, 2, 1, 2, 1, 0, 3, 3, 3, 3,
        2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
        1, 2, 0, 2, 2, 2, 2, 2, 1, 2, 1, 0, 3, 3, 3, 3,
        2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
        2, 2, 0, 2, 2, 2, 2, 2, 1, 2, 1, 0, 3, 3, 3, 3,
        2, 2, 0, 0, 2, 2, 2, 2, 1, 3, 1, 0, 0, 3, 0, 0,
        2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 0, 3, 3, 3, 3,
        2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 0, 3, 3, 3, 3,
        2, 2, 0, 2, 2, 2, 2, 2, 1, 2, 1, 0, 3, 3, 3, 3,
        2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
        2, 2, 0, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
        2, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
    };
    // number of cycles required by each instruction
    static constexpr uint8_t instruction_cycles[256] = {
        7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
        2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
        6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
        2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
        6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
        2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
        6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
        2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
        2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
        2, 6, 2, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,
        2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
        2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
        2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
        2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
        2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
        2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
    };
    // some instructions take an extra cycle if a page is crossed
    static constexpr uint8_t instruction_cycles_extra[256] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
    };
    // private functions
    uint8_t get_flags() const;
    void set_flags(uint8_t);
    uint8_t next_byte();
    uint16_t next_two_bytes();
    void set_ZN_flags(uint8_t);
    void push_stack(uint8_t);
    uint8_t pull_stack();
    void interrupt(InterruptType);
    // instructions
    void adc(const InstructionInfo&);
    void ahx(const InstructionInfo&);
    void alr(const InstructionInfo&);
    void anc(const InstructionInfo&);
    void _and(const InstructionInfo&);
    void arr(const InstructionInfo&);
    void asl(const InstructionInfo&);
    void axs(const InstructionInfo&);
    void bcc(const InstructionInfo&);
    void bcs(const InstructionInfo&);
    void beq(const InstructionInfo&);
    void bit(const InstructionInfo&);
    void bmi(const InstructionInfo&);
    void bne(const InstructionInfo&);
    void bpl(const InstructionInfo&);
    void brk(const InstructionInfo&);
    void bvc(const InstructionInfo&);
    void bvs(const InstructionInfo&);
    void clc(const InstructionInfo&);
    void cld(const InstructionInfo&);
    void cli(const InstructionInfo&);
    void clv(const InstructionInfo&);
    void cmp(const InstructionInfo&);
    void cpx(const InstructionInfo&);
    void cpy(const InstructionInfo&);
    void dcp(const InstructionInfo&);
    void dec(const InstructionInfo&);
    void dex(const InstructionInfo&);
    void dey(const InstructionInfo&);
    void eor(const InstructionInfo&);
    void inc(const InstructionInfo&);
    void inx(const InstructionInfo&);
    void iny(const InstructionInfo&);
    void isb(const InstructionInfo&);
    void jmp(const InstructionInfo&);
    void jsr(const InstructionInfo&);
    void kil(const InstructionInfo&);
    void las(const InstructionInfo&);
    void lax(const InstructionInfo&);
    void lda(const InstructionInfo&);
    void ldx(const InstructionInfo&);
    void ldy(const InstructionInfo&);
    void lsr(const InstructionInfo&);
    void nop(const InstructionInfo&);
    void ora(const InstructionInfo&);
    void pha(const InstructionInfo&);
    void php(const InstructionInfo&);
    void pla(const InstructionInfo&);
    void plp(const InstructionInfo&);
    void rla(const InstructionInfo&);
    void rol(const InstructionInfo&);
    void ror(const InstructionInfo&);
    void rra(const InstructionInfo&);
    void rti(const InstructionInfo&);
    void rts(const InstructionInfo&);
    void sax(const InstructionInfo&);
    void sbc(const InstructionInfo&);
    void sec(const InstructionInfo&);
    void sed(const InstructionInfo&);
    void sei(const InstructionInfo&);
    void shx(const InstructionInfo&);
    void shy(const InstructionInfo&);
    void slo(const InstructionInfo&);
    void sre(const InstructionInfo&);
    void sta(const InstructionInfo&);
    void stx(const InstructionInfo&);
    void sty(const InstructionInfo&);
    void tas(const InstructionInfo&);
    void tax(const InstructionInfo&);
    void tay(const InstructionInfo&);
    void tsx(const InstructionInfo&);
    void txa(const InstructionInfo&);
    void txs(const InstructionInfo&);
    void tya(const InstructionInfo&);
    void xaa(const InstructionInfo&);
};

#endif
