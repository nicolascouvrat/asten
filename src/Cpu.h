#ifndef GUARD_CPU_H
#define GUARD_CPU_H

#include "Memory.h"
#include <iostream>
#include <cstdint>

// info for instructions
struct StepInfo {
    uint16_t pc;
    uint16_t address;
    uint8_t mode;
};

class CPU {
public:
    CPU();
    CPUMemory& get_memory();
    // TODO: remove
    void execute(uint8_t);
private:
    CPUMemory mem;
    uint8_t A, X, Y;                // registers
    uint8_t sp;                     // stack pointer
    uint16_t pc;                    // program counter
    bool C, Z, I, D, B, U, O, N;    // processor flags
    // instruction table
    // define cpu_instruction type
    typedef void (CPU::*cpu_instruction)(const StepInfo&);
    cpu_instruction instruction_table[256];
    // instructions
    void adc(const StepInfo&);
    void ahx(const StepInfo&);
    void alr(const StepInfo&);
    void anc(const StepInfo&);
    void _and(const StepInfo&);
    void arr(const StepInfo&);
    void asl(const StepInfo&);
    void axs(const StepInfo&);
    void bcc(const StepInfo&);
    void bcs(const StepInfo&);
    void beq(const StepInfo&);
    void bit(const StepInfo&);
    void bmi(const StepInfo&);
    void bne(const StepInfo&);
    void bpl(const StepInfo&);
    void brk(const StepInfo&);
    void bvc(const StepInfo&);
    void bvs(const StepInfo&);
    void clc(const StepInfo&);
    void cld(const StepInfo&);
    void cli(const StepInfo&);
    void clv(const StepInfo&);
    void cmp(const StepInfo&);
    void cpx(const StepInfo&);
    void cpy(const StepInfo&);
    void dcp(const StepInfo&);
    void dec(const StepInfo&);
    void dex(const StepInfo&);
    void dey(const StepInfo&);
    void eor(const StepInfo&);
    void inc(const StepInfo&);
    void inx(const StepInfo&);
    void iny(const StepInfo&);
    void isb(const StepInfo&);
    void jmp(const StepInfo&);
    void jsr(const StepInfo&);
    void kil(const StepInfo&);
    void las(const StepInfo&);
    void lax(const StepInfo&);
    void lda(const StepInfo&);
    void ldx(const StepInfo&);
    void ldy(const StepInfo&);
    void lsr(const StepInfo&);
    void nop(const StepInfo&);
    void ora(const StepInfo&);
    void pha(const StepInfo&);
    void php(const StepInfo&);
    void pla(const StepInfo&);
    void plp(const StepInfo&);
    void rla(const StepInfo&);
    void rol(const StepInfo&);
    void ror(const StepInfo&);
    void rra(const StepInfo&);
    void rti(const StepInfo&);
    void rts(const StepInfo&);
    void sax(const StepInfo&);
    void sbc(const StepInfo&);
    void sec(const StepInfo&);
    void sed(const StepInfo&);
    void sei(const StepInfo&);
    void shx(const StepInfo&);
    void shy(const StepInfo&);
    void slo(const StepInfo&);
    void sre(const StepInfo&);
    void sta(const StepInfo&);
    void stx(const StepInfo&);
    void sty(const StepInfo&);
    void tas(const StepInfo&);
    void tax(const StepInfo&);
    void tay(const StepInfo&);
    void tsx(const StepInfo&);
    void txa(const StepInfo&);
    void txs(const StepInfo&);
    void tya(const StepInfo&);
    void xaa(const StepInfo&);
};



#endif
