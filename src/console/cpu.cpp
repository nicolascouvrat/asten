#include "cpu.h"


constexpr uint8_t CPU::instructionModes[];
constexpr uint8_t CPU::instructionCycles[];
constexpr uint8_t CPU::instructionCyclesExtra[];

std::runtime_error notImplementedOp(std::string opcode) {
  return std::runtime_error("Not implemented op: " + opcode);
}

bool pagesDiffer(uint16_t a, uint16_t b) {
    return (a & 0xff00) != (b & 0xff00);
}

/* CONSTRUCTOR */

CPU::CPU(Console& console):
  log(Logger::getLogger("CPU", "cpu.log")),
  mem(console), 
  instructionTable{
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
  cyclesToWait = 0;
  latestInstruction = 0x04; // NOP
  log.setLevel(INFO);
}

/* PUBLIC FUNCTIONS */
CPUStateData CPU::dumpState() {
  CPUStateData data;
  data.A = A;
  data.X = X;
  data.Y = Y;
  data.sp = sp;
  data.pc = pc;
  data.flags = getFlags();
  data.latestInstruction = latestInstruction;
  data.cycle = clock % 341;
  return data;
}

CPUMemory& CPU::getMemory() {
    return mem;
}

void CPU::debugSetPc(uint16_t address) { pc = address; }

void CPU::waitFor(int cycles) { cyclesToWait += cycles; } 

void CPU::fastForwardClock(long ticks) { 
  clock += ticks;
  clock = clock % 341;
}

long CPU::step() {
  log.debug() << dumpState() << "\n";
  if (cyclesToWait > 0) {
    // simulates CPU doing copy op to PPU memory
    cyclesToWait--;
    clock++;
    return 1;
  }
  // read instruction
  uint8_t opcode = nextByte();
  AddressingMode mode = static_cast<AddressingMode>(CPU::instructionModes[opcode]);
  // determine address
  uint16_t address = 0x0000;
  uint16_t temp16, wrappedIncrement;
  uint8_t temp8;
  long startClock = clock;
  bool pageChanged =  false;
  switch (mode) {
    case _:
      // this should not exist
      throw std::runtime_error("Invalid CPU mode");
    case ABSOLUTE_MODE:
      // full memory location is being use as argument
      address = nextTwoBytes();
      break;
    case ABSOLUTEX_MODE:
      // adds the value of X to absolute address
      address = nextTwoBytes() + X;
      pageChanged = pagesDiffer(address, address - X);
      break;
    case ABSOLUTEY_MODE:
      // adds the value of Y to absolute address
      address = nextTwoBytes() + Y;
      pageChanged = pagesDiffer(address, address - Y);
      break;
    case ACCUMULATOR_MODE:
      break;
    case IMMEDIATE_MODE:
      // special mode: here, the address is in fact the value to be used.
      // this will be handled in the operations that support immediate mode.
      // only supports one byte values.
      address = nextByte();
      break;
    case IMPLIED_MODE:
      // special mode: the address is not used.
      address = -1;
      break;
    case INDEXED_INDIRECT_MODE:
      // takes one byte as a one page address, adds X, the generates a 2-byte address
      // force wrap if overflow
      temp8 = nextByte() + X;
      address = mem.read(temp8) | (mem.read((uint8_t)(temp8 + 1)) << 8);
      break;
    case INDIRECT_MODE:
      // look up the first address (on two bytes), 
      // then read two bytes to make up the real address
      temp16 = nextTwoBytes();
      // make sure we do NOT get out of a page with the increment
      wrappedIncrement = (temp16 & 0xff00) +  ((temp16 + 1) & 0x00ff);
      address = mem.read(temp16) | (mem.read(wrappedIncrement) << 8);
      break;
    case INDIRECT_INDEXED_MODE:
      // takes one byte as a one page address, adds Y, the generates a 2-byte address
      // force wrap if overflow
      temp8 = nextByte();
      address = mem.read(temp8) | (mem.read((uint8_t)(temp8 + 1)) << 8);
      address += Y;
      pageChanged = pagesDiffer(address, address - Y);
      break;
    case RELATIVE_MODE:
      // special mode: the address in that case is a single byte and indicates and offset
      // the offset is used as a SIGNED integer!
      temp8 = nextByte();
      if (temp8 > 0x80)
          address = pc + temp8 - 0x100;
      else
          address = pc + temp8;
      break;
    case ZERO_PAGE_MODE:
      // access the first page of memory, next byte is least significant one
      address = nextByte();
      break;
    case ZERO_PAGEX_MODE:
      // adds the value of X to the zero page address
      address = (uint8_t)(nextByte() + X); // force wrap around if overflow
      break;
    case ZERO_PAGEY_MODE:
      // adds the value of Y to the zero page address
      address = (uint8_t)(nextByte() + Y); // force wrap around if overflow
      break;
  }
  // execute instruction
  InstructionInfo info = { address, mode };
  latestInstruction = opcode;
  (this->*instructionTable[opcode])(info);
  // increment clock
  clock += instructionCycles[opcode];
  if (pageChanged)
    clock += instructionCyclesExtra[opcode];
      
  return clock - startClock;
}

void CPU::reset() {
  interrupt(RESET);
  sp = 0xfd;
  setFlags(0b100100);
}

void CPU::triggerNmi() {
  interrupt(NMI);
}

void CPU::triggerIrq() {
  if (I) interrupt(IRQ);
}

/* PRIVATE FUNCTIONS */
void CPU::interrupt(InterruptType interrupt) {
  if (interrupt != RESET) {
    uint8_t currentFlags = getFlags();
    if (interrupt == BRK)
      currentFlags = currentFlags & 0x10; // set B on the copy
    // push lower then higher PC byte on stack
    pushStack(pc >> 8);
    pushStack(pc);
    // save flags
    pushStack(currentFlags);
    // disable interrupts
    I = true;
  }
  // load vector in program counter
  switch(interrupt) {
    case NMI:
      pc = mem.read(0xfffa) | (mem.read(0xfffb) << 8);
      cyclesToWait += 7;
      break;
    case RESET:
      pc = mem.read(0xfffc) | (mem.read(0xfffd) << 8);
      break;
    case BRK:
    case IRQ:
      pc = mem.read(0xfffe) | (mem.read(0xffff) << 8);
      cyclesToWait += 7;
      break;
  }
}

void CPU::pushStack(uint8_t value) {
  mem.write(0x100 + sp--, value);
}

uint8_t CPU::pullStack() {
  return mem.read(0x100 + ++sp);
}

void CPU::setZNFlags(uint8_t value) {
  // After most instructions that have a value, Z and N flags are set according to that value
  Z = value == 0;
  N = value >> 7;
}

uint8_t CPU::nextByte() {
  return mem.read(pc++);
}

uint16_t CPU::nextTwoBytes() {
  // The 6502 uses little endian
  return nextByte() | (nextByte() << 8);
}

uint8_t CPU::getFlags() const {
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

void CPU::setFlags(uint8_t flags) {
  C = (flags >> 0) & 1;
  Z = (flags >> 1) & 1;
  I = (flags >> 2) & 1;
  D = (flags >> 3) & 1;
  B = (flags >> 4) & 1;
  U = 1;
  O = (flags >> 6) & 1;
  N = (flags >> 7) & 1;
}

void CPU::branch(uint16_t newAddress) {
  // branches to new address, handles extra cycles as well
  clock++;
  if (pagesDiffer(pc, newAddress))
    clock++;
  pc = newAddress;
}

/* INSTRUCTIONS */
void CPU::adc(const InstructionInfo& i){
  uint16_t temp;
  uint8_t value;
  bool bothNegative, bothPositive;
  value = (i.mode == IMMEDIATE_MODE) ? i.address : mem.read(i.address);
  bothNegative = (A >> 7) && (value >> 7);
  bothPositive = !(A >> 7) && !(value >> 7);
  
  temp = A + value + C;
  // "cut" to lowest 8 bytes
  A = temp;
  // handle carry flag
  C = (temp > 0xff) ? 1 : 0;
  // a change of sign when both arguments had the same one indicates an overflow
  if ((bothNegative && !(A >> 7)) || (bothPositive && (A >> 7)))
    O = true;
  else
    O = false;
  setZNFlags(A);
}
 
void CPU::ahx(const InstructionInfo& i){
  throw notImplementedOp("ahx");
}

void CPU::alr(const InstructionInfo& i){
  throw notImplementedOp("alr");
}

void CPU::anc(const InstructionInfo& i){
  throw notImplementedOp("anc");
}

void CPU::_and(const InstructionInfo& i){
  A &= (i.mode == IMMEDIATE_MODE) ? i.address : mem.read(i.address);
  setZNFlags(A);
}

void CPU::arr(const InstructionInfo& i){
  throw notImplementedOp("arr");
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
  setZNFlags(temp);
}

void CPU::axs(const InstructionInfo& i){
  throw notImplementedOp("axs");
}

void CPU::bcc(const InstructionInfo& i){
  if (!C) 
    branch(i.address);
}

void CPU::bcs(const InstructionInfo& i){
  if (C) 
    branch(i.address);
}

void CPU::beq(const InstructionInfo& i){
  if (Z)
    branch(i.address);
}

void CPU::bit(const InstructionInfo& i){
  uint8_t tmp = mem.read(i.address);
  N = (tmp >> 7) & 1;
  O = (tmp >> 6) & 1;
  Z = (tmp & A) == 0;
}

void CPU::bmi(const InstructionInfo& i){
  if (N) 
    branch(i.address);
}

void CPU::bne(const InstructionInfo& i){
  if (!Z) 
    branch(i.address);
}

void CPU::bpl(const InstructionInfo& i){
  if (!N) 
    branch(i.address);
}

void CPU::brk(const InstructionInfo& i){
  interrupt(BRK);
}

void CPU::bvc(const InstructionInfo& i){
  if (!O) 
    branch(i.address);
}

void CPU::bvs(const InstructionInfo& i){
  if (O) 
    branch(i.address);
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
  setZNFlags(A - value);
}

void CPU::cpx(const InstructionInfo& i){
  uint8_t value = (i.mode == IMMEDIATE_MODE) ? i.address : mem.read(i.address);
  C = ((X - value) >= 0) ? 1 : 0;
  setZNFlags(X - value);
}

void CPU::cpy(const InstructionInfo& i){
  uint8_t value = (i.mode == IMMEDIATE_MODE) ? i.address : mem.read(i.address);
  C = ((Y - value) >= 0) ? 1 : 0;
  setZNFlags(Y - value);
}

void CPU::dcp(const InstructionInfo& i){
  dec(i);
  cmp(i);
}

void CPU::dec(const InstructionInfo& i){
  uint8_t temp = mem.read(i.address) - 1;
  mem.write(i.address, temp);
  setZNFlags(temp);
}

void CPU::dex(const InstructionInfo& i){
  X = X - 1;
  setZNFlags(X);
}

void CPU::dey(const InstructionInfo& i){
  Y = Y - 1;
  setZNFlags(Y);
}

void CPU::eor(const InstructionInfo& i){
  A ^= (i.mode == IMMEDIATE_MODE) ? i.address : mem.read(i.address);
  setZNFlags(A);
}

void CPU::inc(const InstructionInfo& i){
  uint8_t temp = mem.read(i.address) + 1;
  mem.write(i.address, temp);
  setZNFlags(temp);
}

void CPU::inx(const InstructionInfo& i){
  X = X + 1;
  setZNFlags(X);
}

void CPU::iny(const InstructionInfo& i){
  Y = Y + 1;
  setZNFlags(Y);
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
  pushStack((pc - 1) >> 8);
  pushStack(pc - 1);
  // this is a special case of absolute mode where the address is used to set the pc.
  pc = i.address;
}

void CPU::kil(const InstructionInfo& i){
  throw notImplementedOp("kil");
}

void CPU::las(const InstructionInfo& i){
  throw notImplementedOp("las");
}

void CPU::lax(const InstructionInfo& i){
  lda(i);
  ldx(i);
}

void CPU::lda(const InstructionInfo& i){
  A = (i.mode == IMMEDIATE_MODE) ? i.address : mem.read(i.address);
  setZNFlags(A);
}

void CPU::ldx(const InstructionInfo& i){
  X = (i.mode == IMMEDIATE_MODE) ? i.address : mem.read(i.address);
  setZNFlags(X);
}

void CPU::ldy(const InstructionInfo& i){
  Y = (i.mode == IMMEDIATE_MODE) ? i.address : mem.read(i.address);
  setZNFlags(Y);
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
  setZNFlags(temp);
}

void CPU::nop(const InstructionInfo& i){
  // do nothing
}

void CPU::ora(const InstructionInfo& i){
  A |= (i.mode == IMMEDIATE_MODE) ? i.address : mem.read(i.address);
  setZNFlags(A);
}

void CPU::pha(const InstructionInfo& i){
  pushStack(A);
}

void CPU::php(const InstructionInfo& i){
  pushStack(getFlags() | 0x10); // the B flag is set to true on the stack copy
}

void CPU::pla(const InstructionInfo& i){
  A = pullStack();
  setZNFlags(A);
}

void CPU::plp(const InstructionInfo& i){
  // flag 4 always set to 0 and flag 5 to 1 (handled in setFlags)
  setFlags(pullStack() & 0xcf);
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
  setZNFlags(temp);
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
  setZNFlags(temp);
}

void CPU::rra(const InstructionInfo& i){
  ror(i);
  adc(i);
}

void CPU::rti(const InstructionInfo& i){
  setFlags(pullStack() & 0xcf); // ignore flag 4 and 5
  pc = pullStack() | (pullStack() << 8);
}

void CPU::rts(const InstructionInfo& i){
  pc = (pullStack() | (pullStack() << 8)) + 1;
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
  throw notImplementedOp("tas");
}

void CPU::tax(const InstructionInfo& i){
  X = A;
  setZNFlags(X);
}

void CPU::tay(const InstructionInfo& i){
  Y = A;
  setZNFlags(Y);
}

void CPU::tsx(const InstructionInfo& i){
  X = sp;
  setZNFlags(X);
}

void CPU::txa(const InstructionInfo& i){
  A = X;
  setZNFlags(A);
}

void CPU::txs(const InstructionInfo& i){
  sp = X;
}

void CPU::tya(const InstructionInfo& i){
  A = Y;
  setZNFlags(A);
}

void CPU::xaa(const InstructionInfo& i){
  throw notImplementedOp("xaa");
}
