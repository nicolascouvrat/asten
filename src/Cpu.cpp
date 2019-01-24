#include "Cpu.h"
#include "Logger.h"
// TODO: add warning quand la stack boucle
// QT/GTK/

namespace {
    Logger log = Logger::get_logger("NESCpu").set_level(DEBUG);
}

constexpr uint8_t CPU::instruction_modes[];
constexpr uint8_t CPU::instruction_cycles[];
constexpr uint8_t CPU::instruction_cycles_extra[];

/* CONSTRUCTOR */

CPU::CPU(Console& console): instruction_table{
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
}, console(console), mem(console)
{
    // set initial state
    A = 0;
    X = 0;
    Y = 0;
    sp = 0xfd;
    pc = 0xc000;
    C = false;  // Carry
    Z = false;  // Zero
    I = false;  // Interrupt Disable
    D = false;  // Decimal
    B = true;   // No CPU effect, but used by some instructions
    U = true;   // Unused. Always true
    O = false;  // Overflow
    N = false;  // Negative
    clock = 0;
    latest_instruction = 0x04; // NOP
}

/* PUBLIC FUNCTIONS */
CPUStateData& operator<< (CPUStateData& d, const CPU& c) {
        d.A = c.A;
        d.X = c.X;
        d.Y = c.Y;
        d.sp = c.sp;
        d.pc = c.pc;
        d.flags = c.get_flags();
        d.latest_instruction = c.latest_instruction;
        return d;
}

void CPU::debug_dump() {
    CPUStateData d;
    d << *this;
    log.debug() << d << "\n";
}

CPUMemory& CPU::get_memory() {
    return mem;
}

void CPU::execute(uint8_t opcode) {
    InstructionInfo t = {};
    (this->*instruction_table[opcode])(t);
}

void CPU::step() {
    // read instruction
    uint8_t opcode = next_byte();
    AddressingMode mode = static_cast<AddressingMode>(CPU::instruction_modes[opcode]);
    // determine address
    uint16_t address = 0x0000;
    uint16_t temp16, wrapped_increment;
    uint8_t temp8;
    switch (mode) {
        case ABSOLUTE_MODE:
            // full memory location is being use as argument
            address = next_two_bytes();
            break;
        case ABSOLUTEX_MODE:
            // adds the value of X to absolute address
            address = next_two_bytes() + X;
            break;
        case ABSOLUTEY_MODE:
            // adds the value of Y to absolute address
            address = next_two_bytes() + Y;
            break;
        case ACCUMULATOR_MODE:
            break;
        case IMMEDIATE_MODE:
            // special mode: here, the address is in fact the value to be used.
            // this will be handled in the operations that support immediate mode.
            // only supports one byte values.
            address = next_byte();
            break;
        case IMPLIED_MODE:
            // special mode: the address is not used.
            address = -1;
            break;
        case INDEXED_INDIRECT_MODE:
            // takes one byte as a one page address, adds X, the generates a 2-byte address
            // force wrap if overflow
            temp8 = next_byte() + X;
            address = mem.read(temp8) | (mem.read((uint8_t)(temp8 + 1)) << 8);
            break;
        case INDIRECT_MODE:
            // look up the first address (on two bytes), 
            // then read two bytes to make up the real address
            temp16 = next_two_bytes();
            // make sure we do NOT get out of a page with the increment
            wrapped_increment = (temp16 & 0xff00) +  ((temp16 + 1) & 0x00ff);
            address = mem.read(temp16) | (mem.read(wrapped_increment) << 8);
            break;
        case INDIRECT_INDEXED_MODE:
            // takes one byte as a one page address, adds Y, the generates a 2-byte address
            // force wrap if overflow
            temp8 = next_byte();
            address = mem.read(temp8) | (mem.read((uint8_t)(temp8 + 1)) << 8);
            address += Y;
            break;
        case RELATIVE_MODE:
            // special mode: the address in that case is a single byte and indicates and offset
            // the offset is used as a SIGNED integer!
            temp8 = next_byte();
            if (temp8 > 0x80)
                address = pc + temp8 - 0x100;
            else
                address = pc + temp8;
            break;
        case ZERO_PAGE_MODE:
            // access the first page of memory, next byte is least significant one
            address = next_byte();
            break;
        case ZERO_PAGEX_MODE:
            // adds the value of X to the zero page address
            address = (uint8_t)(next_byte() + X); // force wrap around if overflow
            break;
        case ZERO_PAGEY_MODE:
            // adds the value of Y to the zero page address
            address = (uint8_t)(next_byte() + Y); // force wrap around if overflow
            break;
    }
    // execute instruction
    InstructionInfo info = { address, mode };
    latest_instruction = opcode;
    (this->*instruction_table[opcode])(info);
    // increment clock
    clock += instruction_cycles[opcode];
}

// TODO: check this
void CPU::reset() {
    // TODO: fix
    pc = 0xc000;
    // interrupt(RESET);
    sp = 0xfd;
    set_flags(0b100100);
}

/* PRIVATE FUNCTIONS */
void CPU::interrupt(InterruptType interrupt) {
    if (interrupt != RESET) {
        if (interrupt == BRK)
            B = true;
        // push lower then higher PC byte on stack
        push_stack(pc);
        push_stack(pc >> 8);
        // save flags
        push_stack(get_flags());
        // disable interrupts
        I = true;
    }
    // load vector in program counter
    switch(interrupt) {
        case NMI:
            pc = mem.read(0xfffa) | (mem.read(0xfffb) << 8);
            break;
        case RESET:
            pc = mem.read(0xfffc) | (mem.read(0xfffd) << 8);
            break;
        case BRK:
        case IRQ:
            pc = mem.read(0xfffe) | (mem.read(0xffff) << 8);
            break;
    }
}

void CPU::push_stack(uint8_t value) {
    mem.write(0x100 + sp--, value);
}

uint8_t CPU::pull_stack() {
    return mem.read(0x100 + ++sp);
}

void CPU::set_ZN_flags(uint8_t value) {
    // After most instructions that have a value, Z and N flags are set according to that value
    Z = value == 0;
    N = value >> 7;
}

uint8_t CPU::next_byte() {
    return mem.read(pc++);
}

uint16_t CPU::next_two_bytes() {
    // The 6502 uses little endian
    return next_byte() | (next_byte() << 8);
}

uint8_t CPU::get_flags() const {
    uint8_t flags = 0x00;
    flags |= C << 0;
    flags |= Z << 1;
    flags |= I << 2;
    flags |= D << 3;
    flags |= B << 4;
    flags |= U << 5;
    flags |= O << 6;
    flags |= N << 7;
    return flags;
}

void CPU::set_flags(uint8_t flags) {
    C = (flags >> 0) & 1;
    Z = (flags >> 1) & 1;
    I = (flags >> 2) & 1;
    D = (flags >> 3) & 1;
    B = (flags >> 4) & 1;
    U = 1;
    O = (flags >> 6) & 1;
    N = (flags >> 7) & 1;
}

/* INSTRUCTIONS */
void CPU::adc(const InstructionInfo& i){
    uint16_t temp;
    uint8_t value;
    bool both_negative, both_positive;
    value = (i.mode == IMMEDIATE_MODE) ? i.address : mem.read(i.address);
    both_negative = (A >> 7) && (value >> 7);
    both_positive = !(A >> 7) && !(value >> 7);
    
    temp = A + value + C;
    // "cut" to lowest 8 bytes
    A = temp;
    // handle carry flag
    C = (temp > 0xff) ? 1 : 0;
    // a change of sign when both arguments had the same one indicates an overflow
    if ((both_negative && !(A >> 7)) || (both_positive && (A >> 7)))
        O = true;
    else
        O = false;
    set_ZN_flags(A);
}
 
void CPU::ahx(const InstructionInfo& i){
    log.debug() << "WARNING: UNIMPLEMENTED OP" << "\n";
}

void CPU::alr(const InstructionInfo& i){
    log.debug() << "WARNING: UNIMPLEMENTED OP" << "\n";
}

void CPU::anc(const InstructionInfo& i){
    log.debug() << "WARNING: UNIMPLEMENTED OP" << "\n";
}

void CPU::_and(const InstructionInfo& i){
    A &= (i.mode == IMMEDIATE_MODE) ? i.address : mem.read(i.address);
    set_ZN_flags(A);
}

void CPU::arr(const InstructionInfo& i){
    log.debug() << "WARNING: UNIMPLEMENTED OP" << "\n";
}

void CPU::asl(const InstructionInfo& i){
    // special: if accumulator mode, act on A
    uint8_t temp = (i.mode == ACCUMULATOR_MODE ) ? A : mem.read(i.address);
    C = temp >> 7;
    temp = temp << 1;
    if (i.mode == ACCUMULATOR_MODE)
        A = temp;
    else
        mem.write(i.address, temp);
    set_ZN_flags(temp);
}

void CPU::axs(const InstructionInfo& i){
    log.debug() << "WARNING: UNIMPLEMENTED OP" << "\n";
}

void CPU::bcc(const InstructionInfo& i){
    if (!C) 
        pc = i.address;
}

void CPU::bcs(const InstructionInfo& i){
    if (C) 
        pc = i.address;
}

void CPU::beq(const InstructionInfo& i){
    if (Z)
        pc = i.address;
}

void CPU::bit(const InstructionInfo& i){
    uint8_t tmp = mem.read(i.address);
    N = (tmp >> 7) & 1;
    O = (tmp >> 6) & 1;
    Z = (tmp & A) == 0;
}

void CPU::bmi(const InstructionInfo& i){
    if (N) 
        pc = i.address;
}

void CPU::bne(const InstructionInfo& i){
    if (!Z) 
        pc = i.address;
}

void CPU::bpl(const InstructionInfo& i){
    if (!N) 
        pc = i.address;
}

void CPU::brk(const InstructionInfo& i){
    // TODO: restablish
    // interrupt(BRK);
}

void CPU::bvc(const InstructionInfo& i){
    if (!O) 
        pc = i.address;
}

void CPU::bvs(const InstructionInfo& i){
    if (O) 
        pc = i.address;
}

void CPU::clc(const InstructionInfo& i){
    C = 0;
}

void CPU::cld(const InstructionInfo& i){
    D = 0;
}

void CPU::cli(const InstructionInfo& i){
    I = 0;
}

void CPU::clv(const InstructionInfo& i){
    O = 0;
}

void CPU::cmp(const InstructionInfo& i){
    uint8_t value = (i.mode == IMMEDIATE_MODE) ? i.address : mem.read(i.address);
    // carry set if (A - value) >= 0 in NON SIGNED arithmetic
    C = ((A - value) >= 0) ? 1 : 0;
    set_ZN_flags(A - value);
}

void CPU::cpx(const InstructionInfo& i){
    uint8_t value = (i.mode == IMMEDIATE_MODE) ? i.address : mem.read(i.address);
    C = ((X - value) >= 0) ? 1 : 0;
    set_ZN_flags(X - value);
}

void CPU::cpy(const InstructionInfo& i){
    uint8_t value = (i.mode == IMMEDIATE_MODE) ? i.address : mem.read(i.address);
    C = ((Y - value) >= 0) ? 1 : 0;
    set_ZN_flags(Y - value);
}

void CPU::dcp(const InstructionInfo& i){
    dec(i);
    cmp(i);
}

void CPU::dec(const InstructionInfo& i){
    uint8_t temp = mem.read(i.address) - 1;
    mem.write(i.address, temp);
    set_ZN_flags(temp);
}

void CPU::dex(const InstructionInfo& i){
    X = X - 1;
    set_ZN_flags(X);
}

void CPU::dey(const InstructionInfo& i){
    Y = Y - 1;
    set_ZN_flags(Y);
}

void CPU::eor(const InstructionInfo& i){
    A ^= (i.mode == IMMEDIATE_MODE) ? i.address : mem.read(i.address);
    set_ZN_flags(A);
}

void CPU::inc(const InstructionInfo& i){
    uint8_t temp = mem.read(i.address) + 1;
    mem.write(i.address, temp);
    set_ZN_flags(temp);
}

void CPU::inx(const InstructionInfo& i){
    X = X + 1;
    set_ZN_flags(X);
}

void CPU::iny(const InstructionInfo& i){
    Y = Y + 1;
    set_ZN_flags(Y);
}

void CPU::isb(const InstructionInfo& i){
    inc(i);
    sbc(i);
}

void CPU::jmp(const InstructionInfo& i){
    // this is a special case of absolute mode where the address is used to set the pc.
    pc = i.address;
}

void CPU::jsr(const InstructionInfo& i){
    // the pc already jump to the theoretical next instruction.
    push_stack((pc - 1) >> 8);
    push_stack(pc - 1);
    // this is a special case of absolute mode where the address is used to set the pc.
    pc = i.address;
}

void CPU::kil(const InstructionInfo& i){
    log.debug() << "WARNING: UNIMPLEMENTED OP" << "\n";
}

void CPU::las(const InstructionInfo& i){
    log.debug() << "WARNING: UNIMPLEMENTED OP" << "\n";
}

void CPU::lax(const InstructionInfo& i){
    lda(i);
    ldx(i);
}

void CPU::lda(const InstructionInfo& i){
    A = (i.mode == IMMEDIATE_MODE) ? i.address : mem.read(i.address);
    set_ZN_flags(A);
}

void CPU::ldx(const InstructionInfo& i){
    X = (i.mode == IMMEDIATE_MODE) ? i.address : mem.read(i.address);
    set_ZN_flags(X);
}

void CPU::ldy(const InstructionInfo& i){
    Y = (i.mode == IMMEDIATE_MODE) ? i.address : mem.read(i.address);
    set_ZN_flags(Y);
}

void CPU::lsr(const InstructionInfo& i){
    // special: if accumulator mode, act on A
    uint8_t temp = (i.mode == ACCUMULATOR_MODE) ? A : mem.read(i.address);
    C = temp & 1;
    temp = temp >> 1;
    if (i.mode == ACCUMULATOR_MODE)
        A = temp;
    else
        mem.write(i.address, temp);
    set_ZN_flags(temp);
}

void CPU::nop(const InstructionInfo& i){
    // do nothing
}

void CPU::ora(const InstructionInfo& i){
    A |= (i.mode == IMMEDIATE_MODE) ? i.address : mem.read(i.address);
    set_ZN_flags(A);
}

void CPU::pha(const InstructionInfo& i){
    push_stack(A);
}

void CPU::php(const InstructionInfo& i){
    push_stack(get_flags() | 0x10); // the B flag is set to true on the stack copy
}

void CPU::pla(const InstructionInfo& i){
    A = pull_stack();
    set_ZN_flags(A);
}

void CPU::plp(const InstructionInfo& i){
    // flag 4 always set to 0 and flag 5 to 1 (handled in set_flags)
    set_flags(pull_stack() & 0xcf);
}

void CPU::rla(const InstructionInfo& i){
    rol(i);
    _and(i);
}

void CPU::rol(const InstructionInfo& i){
    // special: if accumulator mode, act on A
    uint8_t temp = (i.mode == ACCUMULATOR_MODE) ? A : mem.read(i.address);
    bool new_C = temp >> 7;
    temp = temp << 1 | C;
    if (i.mode == ACCUMULATOR_MODE)
        A = temp;
    else
        mem.write(i.address, temp);
    C = new_C;
    set_ZN_flags(temp);
}

void CPU::ror(const InstructionInfo& i){
    // special: if accumulator mode, act on A
    uint8_t temp = (i.mode == ACCUMULATOR_MODE) ? A : mem.read(i.address);
    bool new_C = temp & 1;
    temp = temp >> 1 | ((uint8_t) C) << 7;
    if (i.mode == ACCUMULATOR_MODE)
        A = temp;
    else
        mem.write(i.address, temp);
    C = new_C;
    set_ZN_flags(temp);
}

void CPU::rra(const InstructionInfo& i){
    ror(i);
    adc(i);
}

void CPU::rti(const InstructionInfo& i){
    set_flags(pull_stack() & 0xcf); // ignore flag 4 and 5
    pc = pull_stack() | (pull_stack() << 8);
}

void CPU::rts(const InstructionInfo& i){
    pc = (pull_stack() | (pull_stack() << 8)) + 1;
}

void CPU::sax(const InstructionInfo& i){
    mem.write(i.address, A & X);
}

void CPU::sbc(const InstructionInfo& i){
    // sbc(value) = A - value - (1 - C)
    //            = A - (~value + 1) - 1 + C
    //            = adc(~value)
    uint8_t value = (i.mode == IMMEDIATE_MODE) ? i.address : mem.read(i.address);
    adc({ (uint8_t)(~value), IMMEDIATE_MODE });
}

void CPU::sec(const InstructionInfo& i){
    C = 1;
}

void CPU::sed(const InstructionInfo& i){
    D = 1;
}

void CPU::sei(const InstructionInfo& i){
    I = 1;
}

void CPU::shx(const InstructionInfo& i){
    uint8_t temp = mem.read(i.address);
    temp = (temp >> 7) + 1;
    mem.write(i.address, X & temp);
}

void CPU::shy(const InstructionInfo& i){
    uint8_t temp = mem.read(i.address);
    temp = (temp >> 7) + 1;
    mem.write(i.address, Y & temp);
}

void CPU::slo(const InstructionInfo& i){
    asl(i);
    ora(i);
}

void CPU::sre(const InstructionInfo& i){
    lsr(i);
    eor(i);
}

void CPU::sta(const InstructionInfo& i){
    mem.write(i.address, A);
}

void CPU::stx(const InstructionInfo& i){
    mem.write(i.address, X);
}

void CPU::sty(const InstructionInfo& i){
    mem.write(i.address, Y);
}

void CPU::tas(const InstructionInfo& i){
    log.debug() << "WARNING: UNIMPLEMENTED OP" << "\n";
}

void CPU::tax(const InstructionInfo& i){
    X = A;
    set_ZN_flags(X);
}

void CPU::tay(const InstructionInfo& i){
    Y = A;
    set_ZN_flags(Y);
}

void CPU::tsx(const InstructionInfo& i){
    X = sp;
    set_ZN_flags(X);
}

void CPU::txa(const InstructionInfo& i){
    A = X;
    set_ZN_flags(A);
}

void CPU::txs(const InstructionInfo& i){
    sp = X;
}

void CPU::tya(const InstructionInfo& i){
    A = Y;
    set_ZN_flags(A);
}

void CPU::xaa(const InstructionInfo& i){
    log.debug() << "WARNING: UNIMPLEMENTED OP" << "\n";
}
