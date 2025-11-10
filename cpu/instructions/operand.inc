#pragma once

#include "cpu.h"
#include "memory_location.h"
#include "utils.h"

namespace Operand {

template <uint8_t shift>
struct R8 {
  template <typename Bus>
  static RegisterLocation8 get(CPU<Bus>* cpu) {
    const uint8_t reg = (cpu->pc().current_opcode() >> shift) & 7;
    switch (reg) {
      case 0:
        return cpu->registers().B();
      case 1:
        return cpu->registers().C();
      case 2:
        return cpu->registers().D();
      case 3:
        return cpu->registers().E();
      case 4:
        return cpu->registers().H();
      case 5:
        return cpu->registers().L();
      case 6:
        FATAL("Shouldn't get to HL register here");
      case 7:
        return cpu->registers().A();
    }
    FATAL("Shouldn't get here");
  }
};

using R8_53 = R8<3>;
using R8_20 = R8<0>;

struct IMM8 {
  template <typename Bus>
  static const ROMLocation8 get(CPU<Bus>* cpu) {
    auto location = ROMLocation8(cpu->pc().read_u8_at_pc());
    cpu->tick();
    return location;
  }
};

struct IMM16 {
  template <typename Bus>
  static const ROMLocation16 get(CPU<Bus>* cpu) {
    cpu->tick();
    const uint8_t address_low = cpu->pc().read_u8_at_pc();
    cpu->tick();
    const uint8_t address_high = cpu->pc().read_u8_at_pc();
    return ROMLocation16((address_high << 8) | address_low);
  }
};

struct R16_54 {
  template <typename Bus>
  static RegisterLocation16 get(CPU<Bus>* cpu) {
    const uint8_t reg = (cpu->pc().current_opcode() >> 4) & 3;
    switch (reg) {
      case 0:
        return cpu->registers().BC();
      case 1:
        return cpu->registers().DE();
      case 2:
        return cpu->registers().HL();
      case 3:
        return cpu->registers().SP();
    }
    FATAL("Shouldn't get here");
  }
};

struct R16STK_54 {
  template <typename Bus>
  static bool isAF(CPU<Bus>* cpu) {
    return ((cpu->pc().current_opcode() >> 4) & 3) == 3;
  }

  template <typename Bus>
  static RegisterLocation16 get(CPU<Bus>* cpu) {
    const uint8_t reg = (cpu->pc().current_opcode() >> 4) & 3;
    switch (reg) {
      case 0:
        return cpu->registers().BC();
      case 1:
        return cpu->registers().DE();
      case 2:
        return cpu->registers().HL();
      case 3:
        return cpu->registers().AF();
    }
    FATAL("Shouldn't get here");
  }
};

struct R16MEM_54 {
  template <typename Bus>
  static RegisterLocation16 get(CPU<Bus>* cpu) {
    const uint8_t reg = (cpu->pc().current_opcode() >> 4) & 3;
    cpu->tick();  //This is most likely wrong, we probably want to tick after reading the memory.
    switch (reg) {
      case 0:
        return cpu->registers().BC();
      case 1:
        return cpu->registers().DE();
      case 2: {
        RegisterLocation16 address = cpu->registers().HL();
        cpu->registers().HL() = cpu->registers().HL().get() + 1;
        return address;
      }
      case 3: {
        RegisterLocation16 address = cpu->registers().HL();
        cpu->registers().HL() = cpu->registers().HL().get() - 1;
        return address;
      }
    }

    FATAL("Shouldn't get here");
  }
};

struct HLMEM {
  template <typename Bus>
  static MemoryLocation<Bus> get(CPU<Bus>* cpu) {
    auto address = cpu->registers().HL().get();
    cpu->tick();
    return MemoryLocation<Bus>{address, cpu->mc(), cpu->memory_bridge()};
  }
};

struct HLMEM_INC {
  template <typename Bus>
  static MemoryLocation<Bus> get(CPU<Bus>* cpu) {
    RegisterLocation16 address = cpu->registers().HL();
    MemoryLocation<Bus> mem_loc = {address.get(), cpu->mc(), cpu->memory_bridge()};
    cpu->registers().HL() = cpu->registers().HL().get() + 1;
    cpu->tick();
    return mem_loc;
  }
};

struct HLMEM_DEC {
  template <typename Bus>
  static MemoryLocation<Bus> get(CPU<Bus>* cpu) {
    RegisterLocation16 address = cpu->registers().HL();
    MemoryLocation<Bus> mem_loc = {address.get(), cpu->mc(), cpu->memory_bridge()};
    cpu->registers().HL() = cpu->registers().HL().get() - 1;
    cpu->tick();
    return mem_loc;
  }
};

struct BCMEM {
  template <typename Bus>
  static MemoryLocation<Bus> get(CPU<Bus>* cpu) {
    auto address = cpu->registers().BC().get();
    cpu->tick();
    return MemoryLocation<Bus>{address, cpu->mc(), cpu->memory_bridge()};
  }
};

struct DEMEM {
  template <typename Bus>
  static MemoryLocation<Bus> get(CPU<Bus>* cpu) {
    auto address = cpu->registers().DE();
    cpu->tick();
    return MemoryLocation<Bus>{address.get(), cpu->mc(), cpu->memory_bridge()};
  }
};

struct IMM16MEM {
  template <typename Bus>
  static MemoryLocation<Bus> get(CPU<Bus>* cpu) {
    const uint16_t address = cpu->pc().read_u16_at_pc();
    cpu->tick();
    cpu->tick();
    cpu->tick();
    return MemoryLocation<Bus>{address, cpu->mc(), cpu->memory_bridge()};
  }
};

struct IMM8MEM {
  template <typename Bus>
  static MemoryLocation<Bus> get(CPU<Bus>* cpu) {
    const uint16_t address = HIGH_RAM_BASE + cpu->pc().read_u8_at_pc();
    cpu->tick();
    cpu->tick();
    return MemoryLocation<Bus>{address, cpu->mc(), cpu->memory_bridge()};
  }
};

struct CMEM {
  template <typename Bus>
  static MemoryLocation<Bus> get(CPU<Bus>* cpu) {
    const uint16_t address = HIGH_RAM_BASE + cpu->registers().C().get();
    cpu->tick();
    return MemoryLocation<Bus>{address, cpu->mc(), cpu->memory_bridge()};
  }
};

struct A {
  template <typename Bus>
  static RegisterLocation8 get(CPU<Bus>* cpu) {
    return cpu->registers().A();
  }
};

struct HL {
  template <typename Bus>
  static RegisterLocation16 get(CPU<Bus>* cpu) {
    return cpu->registers().HL();
  }
};

struct SP {
  template <typename Bus>
  static RegisterLocation16 get(CPU<Bus>* cpu) {
    auto location = cpu->registers().SP();
    cpu->tick();
    return location;
  }
};

struct B3_53 {
  template <typename Bus>
  static uint8_t get(CPU<Bus>* cpu) {
    return (cpu->pc().current_opcode() >> 3) & 7;
  }
};

struct COND_43 {
  template <typename Bus>
  static bool get(CPU<Bus>* cpu) {
    uint8_t bit = (cpu->pc().current_opcode() >> 3) & 3;
    switch (bit) {
      case 0:
        VERBOSE_PRINT() << "COND_43: !zero_flag";
        return !cpu->registers().get_zero_flag();
      case 1:
        VERBOSE_PRINT() << "COND_43: zero_flag";
        return cpu->registers().get_zero_flag();
      case 2:
        VERBOSE_PRINT() << "COND_43: !carry_flag";
        return !cpu->registers().get_carry_flag();
      case 3:
        VERBOSE_PRINT() << "COND_43: carry_flag";
        return cpu->registers().get_carry_flag();
      default:
        FATAL("Shouldn't get here");
    }
  }
};

struct TGT3_53 {
  template <typename Bus>
  static ROMLocation16 get(CPU<Bus>* cpu) {
    return ROMLocation16(8 * ((cpu->pc().current_opcode() >> 3) & 7));
  }
};
};  // namespace Operand
