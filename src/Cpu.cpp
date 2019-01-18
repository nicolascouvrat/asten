#include "Cpu.h"

/* CONSTRUCTOR */

CPU::CPU(): instruction_table{
    &CPU::brk, &CPU::ora, &CPU::kil, &CPU::slo, &CPU::nop, &CPU::ora, &CPU::asl, &CPU::slo,
    &CPU::php, &CPU::ora, &CPU::asl, &CPU::anc, &CPU::nop, &CPU::ora, &CPU::asl, &CPU::slo,
    &CPU::bpl, &CPU::ora, &CPU::kil, &CPU::slo, &CPU::nop, &CPU::ora, &CPU::asl, &CPU::slo,
    &CPU::clc, &CPU::ora, &CPU::nop, &CPU::slo, &CPU::nop, &CPU::ora, &CPU::asl, &CPU::slo,
    &CPU::jsr, &CPU::_and, &CPU::kil, &CPU::rla, &CPU::bit, &CPU::_and, &CPU::rol, &CPU::rla,
    &CPU::plp, &CPU::_and, &CPU::rol, &CPU::anc, &CPU::bit, &CPU::_and, &CPU::rol, &CPU::rla,
    &CPU::bmi, &CPU::_and, &CPU::kil, &CPU::rla, &CPU::nop, &CPU::_and, &CPU::rol, &CPU::rla,
    &CPU::sec, &CPU::_and, &CPU::nop, &CPU::rla, &CPU::nop, &CPU::_and, &CPU::rol, &CPU::rla,
    &CPU::rti, &CPU::eor, &CPU::kil, &CPU::sre, &CPU::nop, &CPU::eor, &CPU::lsr, &CPU::sre,
    &CPU::pha, &CPU::eor, &CPU::lsr, &CPU::alr, &CPU::jmp, &CPU::eor, &CPU::lsr, &CPU::sre,
    &CPU::bvc, &CPU::eor, &CPU::kil, &CPU::sre, &CPU::nop, &CPU::eor, &CPU::lsr, &CPU::sre,
    &CPU::cli, &CPU::eor, &CPU::nop, &CPU::sre, &CPU::nop, &CPU::eor, &CPU::lsr, &CPU::sre,
    &CPU::rts, &CPU::adc, &CPU::kil, &CPU::rra, &CPU::nop, &CPU::adc, &CPU::ror, &CPU::rra,
    &CPU::pla, &CPU::adc, &CPU::ror, &CPU::arr, &CPU::jmp, &CPU::adc, &CPU::ror, &CPU::rra,
    &CPU::bvs, &CPU::adc, &CPU::kil, &CPU::rra, &CPU::nop, &CPU::adc, &CPU::ror, &CPU::rra,
    &CPU::sei, &CPU::adc, &CPU::nop, &CPU::rra, &CPU::nop, &CPU::adc, &CPU::ror, &CPU::rra,
    &CPU::nop, &CPU::sta, &CPU::nop, &CPU::sax, &CPU::sty, &CPU::sta, &CPU::stx, &CPU::sax,
    &CPU::dey, &CPU::nop, &CPU::txa, &CPU::xaa, &CPU::sty, &CPU::sta, &CPU::stx, &CPU::sax,
    &CPU::bcc, &CPU::sta, &CPU::kil, &CPU::ahx, &CPU::sty, &CPU::sta, &CPU::stx, &CPU::sax,
    &CPU::tya, &CPU::sta, &CPU::txs, &CPU::tas, &CPU::shy, &CPU::sta, &CPU::shx, &CPU::ahx,
    &CPU::ldy, &CPU::lda, &CPU::ldx, &CPU::lax, &CPU::ldy, &CPU::lda, &CPU::ldx, &CPU::lax,
    &CPU::tay, &CPU::lda, &CPU::tax, &CPU::lax, &CPU::ldy, &CPU::lda, &CPU::ldx, &CPU::lax,
    &CPU::bcs, &CPU::lda, &CPU::kil, &CPU::lax, &CPU::ldy, &CPU::lda, &CPU::ldx, &CPU::lax,
    &CPU::clv, &CPU::lda, &CPU::tsx, &CPU::las, &CPU::ldy, &CPU::lda, &CPU::ldx, &CPU::lax,
    &CPU::cpy, &CPU::cmp, &CPU::nop, &CPU::dcp, &CPU::cpy, &CPU::cmp, &CPU::dec, &CPU::dcp,
    &CPU::iny, &CPU::cmp, &CPU::dex, &CPU::axs, &CPU::cpy, &CPU::cmp, &CPU::dec, &CPU::dcp,
    &CPU::bne, &CPU::cmp, &CPU::kil, &CPU::dcp, &CPU::nop, &CPU::cmp, &CPU::dec, &CPU::dcp,
    &CPU::cld, &CPU::cmp, &CPU::nop, &CPU::dcp, &CPU::nop, &CPU::cmp, &CPU::dec, &CPU::dcp,
    &CPU::cpx, &CPU::sbc, &CPU::nop, &CPU::isb, &CPU::cpx, &CPU::sbc, &CPU::inc, &CPU::isb,
    &CPU::inx, &CPU::sbc, &CPU::nop, &CPU::sbc, &CPU::cpx, &CPU::sbc, &CPU::inc, &CPU::isb,
    &CPU::beq, &CPU::sbc, &CPU::kil, &CPU::isb, &CPU::nop, &CPU::sbc, &CPU::inc, &CPU::isb,
    &CPU::sed, &CPU::sbc, &CPU::nop, &CPU::isb, &CPU::nop, &CPU::sbc, &CPU::inc, &CPU::isb,
}
{
    // set initial state
    A = 0;
    X = 0;
    Y = 0;
    sp = 0xff;
    pc = 0x6000;
    C = false;
    Z = false;
    I = false;
    D = false;
    B = false;
    U = false;
    O = false;
    N = false;
}

/* PUBLIC FUNCTIONS */

CPUMemory& CPU::get_memory() {
    return mem;
}

void CPU::execute(uint8_t opcode) {
    StepInfo t = {};
    (this->*instruction_table[opcode])(t);
    
}

/* INSTRUCTIONS */
void CPU::adc(const StepInfo&){} 
void CPU::ahx(const StepInfo&){}
void CPU::alr(const StepInfo&){}
void CPU::anc(const StepInfo&){}
void CPU::_and(const StepInfo&){}
void CPU::arr(const StepInfo&){}
void CPU::asl(const StepInfo&){}
void CPU::axs(const StepInfo&){}
void CPU::bcc(const StepInfo&){}
void CPU::bcs(const StepInfo&){}
void CPU::beq(const StepInfo&){}
void CPU::bit(const StepInfo&){}
void CPU::bmi(const StepInfo&){}
void CPU::bne(const StepInfo&){}
void CPU::bpl(const StepInfo&){}
void CPU::brk(const StepInfo&){
    std::cout << "BRK";
}
void CPU::bvc(const StepInfo&){}
void CPU::bvs(const StepInfo&){}
void CPU::clc(const StepInfo&){}
void CPU::cld(const StepInfo&){}
void CPU::cli(const StepInfo&){}
void CPU::clv(const StepInfo&){}
void CPU::cmp(const StepInfo&){}
void CPU::cpx(const StepInfo&){}
void CPU::cpy(const StepInfo&){}
void CPU::dcp(const StepInfo&){}
void CPU::dec(const StepInfo&){}
void CPU::dex(const StepInfo&){}
void CPU::dey(const StepInfo&){}
void CPU::eor(const StepInfo&){}
void CPU::inc(const StepInfo&){}
void CPU::inx(const StepInfo&){}
void CPU::iny(const StepInfo&){}
void CPU::isb(const StepInfo&){}
void CPU::jmp(const StepInfo&){}
void CPU::jsr(const StepInfo&){}
void CPU::kil(const StepInfo&){}
void CPU::las(const StepInfo&){}
void CPU::lax(const StepInfo&){}
void CPU::lda(const StepInfo&){}
void CPU::ldx(const StepInfo&){}
void CPU::ldy(const StepInfo&){}
void CPU::lsr(const StepInfo&){}
void CPU::nop(const StepInfo&){}
void CPU::ora(const StepInfo&){}
void CPU::pha(const StepInfo&){}
void CPU::php(const StepInfo&){}
void CPU::pla(const StepInfo&){}
void CPU::plp(const StepInfo&){}
void CPU::rla(const StepInfo&){}
void CPU::rol(const StepInfo&){}
void CPU::ror(const StepInfo&){}
void CPU::rra(const StepInfo&){}
void CPU::rti(const StepInfo&){}
void CPU::rts(const StepInfo&){}
void CPU::sax(const StepInfo&){}
void CPU::sbc(const StepInfo&){}
void CPU::sec(const StepInfo&){}
void CPU::sed(const StepInfo&){}
void CPU::sei(const StepInfo&){}
void CPU::shx(const StepInfo&){}
void CPU::shy(const StepInfo&){}
void CPU::slo(const StepInfo&){}
void CPU::sre(const StepInfo&){}
void CPU::sta(const StepInfo&){}
void CPU::stx(const StepInfo&){}
void CPU::sty(const StepInfo&){}
void CPU::tas(const StepInfo&){}
void CPU::tax(const StepInfo&){}
void CPU::tay(const StepInfo&){}
void CPU::tsx(const StepInfo&){}
void CPU::txa(const StepInfo&){}
void CPU::txs(const StepInfo&){}
void CPU::tya(const StepInfo&){}
void CPU::xaa(const StepInfo&){}

