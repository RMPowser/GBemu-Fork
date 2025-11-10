#pragma once

#include <sys/types.h>
#include <cstdint>
#include <type_traits>
#include "cpu.h"
#include "flag_ops.h"
#include "operand.inc"
#include "utils.h"

namespace Operator {
struct NOP {
  template <typename Bus>
  void operator()(CPU<Bus>* cpu) {
    VERBOSE_PRINT() << "NOP" << std::endl;
  }
};

struct HALT {
  template <typename Bus>
  void operator()(CPU<Bus>* cpu) {
    VERBOSE_PRINT() << "HALT" << std::endl;
    cpu->halt();
  }
};

struct STOP {
  template <typename Bus>
  void operator()(CPU<Bus>* cpu) {
    VERBOSE_PRINT() << "STOP" << std::endl;
    cpu->stop();
  }
};

struct DI {
  template <typename Bus>
  void operator()(CPU<Bus>* cpu) {
    VERBOSE_PRINT() << "DI" << std::endl;
    cpu->disable_interrupts();
  }
};

struct EI {
  template <typename Bus>
  void operator()(CPU<Bus>* cpu) {
    VERBOSE_PRINT() << "EI" << std::endl;
    cpu->enable_interrupts();
  }
};

struct LD {
  template <typename Bus, typename FirstOperand, typename SecondOperand>
  void operator()(CPU<Bus>* cpu) {
    //These are here because they have side effects, potentially increasing HL
    auto first = FirstOperand::template get<Bus>(cpu);
    auto second = SecondOperand::template get<Bus>(cpu);
    VERBOSE_PRINT() << "LD: " << first.address_str() << " = " << second << std::endl;
    first = second;
  }
};

struct LD_HL_SP {
  template <typename Bus, typename Operand>
  void operator()(CPU<Bus>* cpu) {
    cpu->tick();
    int8_t offset = static_cast<int8_t>(Operand::template get<Bus>(cpu).get());
    VERBOSE_PRINT() << "LD_HL_SP = HL = " << cpu->registers().SP().get() << " + "
                    << static_cast<int16_t>(offset) << std::endl;
    auto result = static_cast<uint16_t>(cpu->registers().SP().get() + static_cast<int16_t>(offset));

    cpu->registers().HL() = result;
    cpu->registers().set_zero_flag(false);
    cpu->registers().set_subtraction_flag(false);
    cpu->registers().set_half_carry_flag(
        FlagOps::check_half_carry_addsp(cpu->registers().SP().get(), offset));
    cpu->registers().set_carry_flag(FlagOps::check_carry_addsp(cpu->registers().SP().get(), offset));
  }
};

struct PUSH {
  template <typename Bus, typename Operand>
  void operator()(CPU<Bus>* cpu) {
    auto operand = Operand::template get<Bus>(cpu);
    VERBOSE_PRINT() << "PUSH: " << operand.address_str() << std::endl;
    cpu->tick();
    cpu->tick();
    cpu->stack().push_8(operand.get() >> 8);

    cpu->tick();
    cpu->stack().push_8(operand.get() & 0xFF);
  }
};

struct POP {
  template <typename Bus, typename Operand>
  void operator()(CPU<Bus>* cpu) {
    cpu->tick();
    const uint8_t value = cpu->stack().pop_8();
    cpu->tick();
    const uint8_t value2 = cpu->stack().pop_8();
    const uint16_t new_value = (value2 << 8) | value;
    auto operand = Operand::template get<Bus>(cpu);
    VERBOSE_PRINT() << "POP: " << operand << " = " << new_value << std::endl;

    operand = new_value;

    if (Operand::template isAF<Bus>(cpu)) {
      cpu->registers().F() = cpu->registers().F().get() & 0xF0;
    }
  }
};

struct ADD {
  template <typename Bus, typename FirstOperand, typename SecondOperand, typename FlagOp>
  void operator()(CPU<Bus>* cpu) {
    auto first_op = FirstOperand::template get<Bus>(cpu);
    auto second_op = SecondOperand::template get<Bus>(cpu);

    VERBOSE_PRINT() << "ADD: " << first_op << " += " << static_cast<uint16_t>(second_op.get()) << std::endl;

    using value_type = std::conditional_t<std::is_same_v<FlagOp, FlagOps::ADD8FLAGS>, uint8_t, uint16_t>;
    const value_type first = first_op.get();
    const value_type second = second_op.get();
    const value_type result = first + second;
    cpu->registers().set_subtraction_flag(false);

    if constexpr (std::is_same_v<FlagOp, FlagOps::ADD8FLAGS>) {
      cpu->registers().set_zero_flag(result == 0);
      cpu->registers().set_half_carry_flag(FlagOps::check_half_carry_add8(first, second));
      cpu->registers().set_carry_flag(FlagOps::check_carry_add8(first, second));
    } else if constexpr (std::is_same_v<FlagOp, FlagOps::ADD16FLAGS>) {
      cpu->registers().set_half_carry_flag(FlagOps::check_half_carry_add16(first, second));
      cpu->registers().set_carry_flag(FlagOps::check_carry_add16(first, second));
      cpu->tick();
    }

    first_op = result;
  }
};

struct ADD_SP {
  template <typename Bus, typename FirstOperand, typename SecondOperand>
  void operator()(CPU<Bus>* cpu) {
    cpu->tick();
    int8_t second = static_cast<int8_t>(SecondOperand::template get<Bus>(cpu).get());

    auto first_op = FirstOperand::template get<Bus>(cpu);

    const uint16_t first = first_op.get();
    const uint16_t result = static_cast<uint16_t>(first) + second;

    VERBOSE_PRINT() << "ADD_SP: " << first << " + " << static_cast<int16_t>(second) << " = " << result
                    << std::endl;

    cpu->registers().set_subtraction_flag(false);
    cpu->registers().set_zero_flag(0);
    cpu->registers().set_half_carry_flag(FlagOps::check_half_carry_addsp(first, second));
    cpu->registers().set_carry_flag(FlagOps::check_carry_addsp(first, second));

    first_op = result;
  }
};

struct ADC {
  template <typename Bus, typename FirstOperand, typename SecondOperand>
  void operator()(CPU<Bus>* cpu) {
    auto first_op = FirstOperand::template get<Bus>(cpu);
    auto second_op = SecondOperand::template get<Bus>(cpu);
    VERBOSE_PRINT() << "ADC: " << first_op << " += " << second_op << " + carry" << std::endl;
    const uint8_t first = first_op.get();
    const uint8_t second = second_op.get();
    const uint8_t result = first + second + cpu->registers().get_carry_flag();

    cpu->registers().set_subtraction_flag(false);
    cpu->registers().set_zero_flag(result == 0);
    cpu->registers().set_half_carry_flag(
        FlagOps::check_half_carry_add8(first, second, cpu->registers().get_carry_flag()));
    cpu->registers().set_carry_flag(
        FlagOps::check_carry_add8(first, second, cpu->registers().get_carry_flag()));

    cpu->registers().A() = result;
  }
};

struct SUB {
  template <typename Bus, typename Operand>
  void operator()(CPU<Bus>* cpu) {
    auto operand = Operand::template get<Bus>(cpu);
    VERBOSE_PRINT() << "SUB: A -= " << operand << std::endl;
    uint8_t a = cpu->registers().A().get();
    uint8_t value = operand.get();
    auto result = a - value;

    cpu->registers().set_subtraction_flag(true);
    cpu->registers().set_zero_flag(result == 0);
    cpu->registers().set_half_carry_flag(FlagOps::check_half_carry_sub(a, value));
    cpu->registers().set_carry_flag(FlagOps::check_carry_sub(a, value));

    cpu->registers().A() = result;
  }
};

struct SBC {
  template <typename Bus, typename FirstOperand, typename SecondOperand>
  void operator()(CPU<Bus>* cpu) {
    auto first_op = FirstOperand::template get<Bus>(cpu);
    auto second_op = SecondOperand::template get<Bus>(cpu);
    VERBOSE_PRINT() << "SBC: " << first_op.address_str() << " -= " << second_op << " + carry" << std::endl;
    uint8_t first = first_op.get();
    uint8_t second = second_op.get();
    uint8_t result = (first - second) - static_cast<uint8_t>(cpu->registers().get_carry_flag());

    cpu->registers().set_subtraction_flag(true);
    cpu->registers().set_zero_flag(result == 0);
    cpu->registers().set_half_carry_flag(
        FlagOps::check_half_carry_sub(first, second) ||
        FlagOps::check_half_carry_sub(first - second, cpu->registers().get_carry_flag()));
    cpu->registers().set_carry_flag(
        FlagOps::check_carry_sub(first, second) ||
        FlagOps::check_carry_sub(first - second, cpu->registers().get_carry_flag()));

    cpu->registers().A() = result;
  }
};

struct AND {
  template <typename Bus, typename Operand>
  void operator()(CPU<Bus>* cpu) {
    auto operand = Operand::template get<Bus>(cpu);
    VERBOSE_PRINT() << "AND: A &= " << operand << std::endl;
    cpu->registers().A() = cpu->registers().A().get() & operand.get();

    cpu->registers().set_zero_flag(cpu->registers().A().get() == 0);
    cpu->registers().set_subtraction_flag(false);
    cpu->registers().set_half_carry_flag(true);
    cpu->registers().set_carry_flag(false);
  }
};

struct XOR {
  template <typename Bus, typename Operand>
  void operator()(CPU<Bus>* cpu) {
    auto operand = Operand::template get<Bus>(cpu);
    VERBOSE_PRINT() << "XOR: A ^= " << operand << std::endl;
    cpu->registers().A() = cpu->registers().A().get() ^ operand.get();

    cpu->registers().set_zero_flag(cpu->registers().A().get() == 0);
    cpu->registers().set_subtraction_flag(false);
    cpu->registers().set_half_carry_flag(false);
    cpu->registers().set_carry_flag(false);
  }
};

struct OR {
  template <typename Bus, typename Operand>
  void operator()(CPU<Bus>* cpu) {
    auto operand = Operand::template get<Bus>(cpu);
    VERBOSE_PRINT() << "OR: A |= " << operand << std::endl;
    cpu->registers().A() = cpu->registers().A().get() | operand.get();

    cpu->registers().set_zero_flag(cpu->registers().A().get() == 0);
    cpu->registers().set_subtraction_flag(false);
    cpu->registers().set_half_carry_flag(false);
    cpu->registers().set_carry_flag(false);
  }
};

struct CP {
  template <typename Bus, typename Operand>
  void operator()(CPU<Bus>* cpu) {
    auto operand = Operand::template get<Bus>(cpu);
    VERBOSE_PRINT() << "CP: A ? " << operand << std::endl;
    const uint8_t a = cpu->registers().A().get();
    const uint8_t value = operand.get();
    const uint8_t result = a - value;

    cpu->registers().set_zero_flag(result == 0);
    cpu->registers().set_subtraction_flag(true);
    cpu->registers().set_half_carry_flag(FlagOps::check_half_carry_sub(a, value));
    cpu->registers().set_carry_flag(FlagOps::check_carry_sub(a, value));
  }
};

struct INC {
  template <typename Bus, typename Operand, typename FlagOp>
  void operator()(CPU<Bus>* cpu) {
    auto operand = Operand::template get<Bus>(cpu);
    auto value = operand.get();
    VERBOSE_PRINT() << "INC: ++" << operand.address_str() << " = " << value + 1 << std::endl;

    if constexpr (std::is_same_v<Operand, typename ::Operand::R16_54>) {
      cpu->tick();
    }

    if constexpr (std::is_same_v<FlagOp, FlagOps::SET>) {
      cpu->registers().set_half_carry_flag(FlagOps::check_half_carry_inc(value));
      cpu->registers().set_subtraction_flag(false);
      cpu->registers().set_zero_flag(FlagOps::check_zero_flag_inc(value));
    }

    Operand::template get<Bus>(cpu) = value + 1;
  }
};

struct DEC {
  template <typename Bus, typename Operand, typename FlagOp>
  void operator()(CPU<Bus>* cpu) {
    auto operand = Operand::template get<Bus>(cpu);
    auto value = operand.get();
    VERBOSE_PRINT() << "DEC: --" << operand.address_str() << " = " << value - 1 << std::endl;

    if constexpr (std::is_same_v<Operand, typename ::Operand::R16_54>) {
      cpu->tick();
    }
    if constexpr (std::is_same_v<FlagOp, FlagOps::SET>) {
      cpu->registers().set_half_carry_flag(FlagOps::check_half_carry_dec(value));
      cpu->registers().set_subtraction_flag(true);
      cpu->registers().set_zero_flag(FlagOps::check_zero_flag_dec(value));
    }

    Operand::template get<Bus>(cpu) = value - 1;
  }
};

struct DAA {
  template <typename Bus>
  void operator()(CPU<Bus>* cpu) {
    VERBOSE_PRINT() << "DAA" << std::endl;

    uint16_t a = cpu->registers().A().get();
    const bool subtraction_flag = cpu->registers().get_subtraction_flag();
    const bool half_carry_flag = cpu->registers().get_half_carry_flag();
    const bool carry_flag = cpu->registers().get_carry_flag();

    if (!subtraction_flag) {
      if (half_carry_flag || (a & 0x0F) > 0x09) {
        a += 0x06;
      }
      if (carry_flag || a > 0x9F) {
        a += 0x60;
      }
    } else {
      if (half_carry_flag) {
        a = (a - 0x06) & 0xFF;
      }
      if (carry_flag) {
        a -= 0x60;
      }
    }

    if ((a & 0x100) == 0x100) {
      cpu->registers().set_carry_flag(true);
    }
    cpu->registers().set_half_carry_flag(false);
    cpu->registers().set_zero_flag((a & 0xFF) == 0);
    cpu->registers().A() = static_cast<uint8_t>(a);
  }
};

struct CPL {
  template <typename Bus>
  void operator()(CPU<Bus>* cpu) {
    VERBOSE_PRINT() << "CPL" << std::endl;
    cpu->registers().A() = ~cpu->registers().A().get();
    cpu->registers().set_subtraction_flag(true);
    cpu->registers().set_half_carry_flag(true);
  }
};

struct CCF {
  template <typename Bus>
  void operator()(CPU<Bus>* cpu) {
    VERBOSE_PRINT() << "CCF" << std::endl;
    cpu->registers().set_carry_flag(!cpu->registers().get_carry_flag());
    cpu->registers().set_half_carry_flag(false);
    cpu->registers().set_subtraction_flag(false);
  }
};

struct SCF {
  template <typename Bus>
  void operator()(CPU<Bus>* cpu) {
    VERBOSE_PRINT() << "SCF" << std::endl;
    cpu->registers().set_carry_flag(true);
    cpu->registers().set_half_carry_flag(false);
    cpu->registers().set_subtraction_flag(false);
  }
};

struct RLCA {
  template <typename Bus>
  void operator()(CPU<Bus>* cpu) {
    VERBOSE_PRINT() << "RLCA" << std::endl;
    cpu->registers().set_carry_flag((cpu->registers().A().get() & 0x80) >> 7);
    cpu->registers().A() = (cpu->registers().A().get() << 1) | (cpu->registers().A().get() >> 7);
    cpu->registers().set_half_carry_flag(false);
    cpu->registers().set_subtraction_flag(false);
    cpu->registers().set_zero_flag(false);
  }
};

struct RLA {
  template <typename Bus>
  void operator()(CPU<Bus>* cpu) {
    VERBOSE_PRINT() << "RLA" << std::endl;
    const bool carry_flag = cpu->registers().get_carry_flag();
    cpu->registers().set_carry_flag((cpu->registers().A().get() & 0x80) >> 7);
    cpu->registers().A() = (cpu->registers().A().get() << 1) + static_cast<uint8_t>(carry_flag);
    cpu->registers().set_half_carry_flag(false);
    cpu->registers().set_subtraction_flag(false);
    cpu->registers().set_zero_flag(false);
  }
};

struct RRCA {
  template <typename Bus>
  void operator()(CPU<Bus>* cpu) {
    VERBOSE_PRINT() << "RRCA" << std::endl;
    const uint8_t a = cpu->registers().A().get();
    const uint8_t result = (a >> 1) | ((a & 1) << 7);
    cpu->registers().set_subtraction_flag(false);
    cpu->registers().set_zero_flag(false);
    cpu->registers().set_half_carry_flag(false);
    cpu->registers().set_carry_flag(a & 1);
    cpu->registers().A() = result;
  }
};

struct RRA {
  template <typename Bus>
  void operator()(CPU<Bus>* cpu) {
    VERBOSE_PRINT() << "RRA" << std::endl;
    const uint8_t result =
        (cpu->registers().A().get() >> 1) | (static_cast<uint8_t>(cpu->registers().get_carry_flag()) << 7);

    cpu->registers().set_subtraction_flag(false);
    cpu->registers().set_zero_flag(false);
    cpu->registers().set_half_carry_flag(false);
    cpu->registers().set_carry_flag(cpu->registers().A().get() & 1);

    cpu->registers().A() = result;
  }
};

struct RLC {
  template <typename Bus, typename Operand>
  void operator()(CPU<Bus>* cpu) {
    auto operand = Operand::template get<Bus>(cpu);
    VERBOSE_PRINT() << "RLC: " << operand.address_str() << std::endl;

    uint8_t value = operand.get();
    if constexpr (std::is_same_v<Operand, typename ::Operand::HLMEM>)
      cpu->tick();
    uint8_t result = (value << 1) | (value >> 7);

    cpu->registers().set_subtraction_flag(false);
    cpu->registers().set_zero_flag(result == 0);
    cpu->registers().set_half_carry_flag(false);
    cpu->registers().set_carry_flag(value & 0x80);

    operand = result;
  }
};

struct RL {
  template <typename Bus, typename Operand>
  void operator()(CPU<Bus>* cpu) {
    auto operand = Operand::template get<Bus>(cpu);
    VERBOSE_PRINT() << "RL: " << operand.address_str() << std::endl;
    const bool carry_flag = cpu->registers().get_carry_flag();
    const uint8_t reg = operand.get();
    const uint8_t result = (reg << 1) | carry_flag;
    if constexpr (std::is_same_v<Operand, typename ::Operand::HLMEM>)
      cpu->tick();
    cpu->registers().set_carry_flag(reg & 0x80);
    cpu->registers().set_half_carry_flag(false);
    cpu->registers().set_subtraction_flag(false);
    cpu->registers().set_zero_flag(result == 0);

    operand = result;
  }
};

struct RRC {
  template <typename Bus, typename Operand>
  void operator()(CPU<Bus>* cpu) {
    auto operand = Operand::template get<Bus>(cpu);
    VERBOSE_PRINT() << "RRC: " << operand.address_str() << std::endl;
    const uint8_t reg = operand.get();
    const uint8_t result = (reg >> 1) | ((reg << 7) & 0x80);
    if constexpr (std::is_same_v<Operand, typename ::Operand::HLMEM>)
      cpu->tick();
    cpu->registers().set_carry_flag(reg & 0x01);
    cpu->registers().set_half_carry_flag(false);
    cpu->registers().set_subtraction_flag(false);
    cpu->registers().set_zero_flag(result == 0);

    operand = result;
  }
};

struct RR {
  template <typename Bus, typename Operand>
  void operator()(CPU<Bus>* cpu) {
    auto operand = Operand::template get<Bus>(cpu);
    VERBOSE_PRINT() << "RR: " << operand.address_str() << std::endl;
    uint8_t value = operand.get();
    uint8_t result = (value >> 1) | (static_cast<uint8_t>(cpu->registers().get_carry_flag()) << 7);
    if constexpr (std::is_same_v<Operand, typename ::Operand::HLMEM>)
      cpu->tick();
    cpu->registers().set_subtraction_flag(false);
    cpu->registers().set_zero_flag(result == 0);
    cpu->registers().set_half_carry_flag(false);
    cpu->registers().set_carry_flag(value & 1);

    operand = result;
  }
};

struct SLA {
  template <typename Bus, typename Operand>
  void operator()(CPU<Bus>* cpu) {
    auto operand = Operand::template get<Bus>(cpu);
    VERBOSE_PRINT() << "SLA: " << operand.address_str() << std::endl;
    uint8_t value = operand.get();
    cpu->registers().set_carry_flag(value & 0x80);
    uint8_t result = value << 1;
    if constexpr (std::is_same_v<Operand, typename ::Operand::HLMEM>)
      cpu->tick();
    operand = result;
    cpu->registers().set_half_carry_flag(false);
    cpu->registers().set_subtraction_flag(false);
    cpu->registers().set_zero_flag(result == 0);
  }
};

struct SRA {
  template <typename Bus, typename Operand>
  void operator()(CPU<Bus>* cpu) {
    auto operand = Operand::template get<Bus>(cpu);
    VERBOSE_PRINT() << "SRA: " << operand.address_str() << std::endl;
    uint8_t value = operand.get();
    cpu->registers().set_carry_flag(value & 0x01);
    uint8_t result = (value >> 1) | (value & 0x80);
    if constexpr (std::is_same_v<Operand, typename ::Operand::HLMEM>)
      cpu->tick();
    operand = result;
    cpu->registers().set_half_carry_flag(false);
    cpu->registers().set_subtraction_flag(false);
    cpu->registers().set_zero_flag(result == 0);
  }
};

struct SRL {
  template <typename Bus, typename Operand>
  void operator()(CPU<Bus>* cpu) {
    auto operand = Operand::template get<Bus>(cpu);
    VERBOSE_PRINT() << "SRL: " << operand.address_str() << std::endl;
    uint8_t value = operand.get();
    uint8_t result = value >> 1;
    if constexpr (std::is_same_v<Operand, typename ::Operand::HLMEM>)
      cpu->tick();

    cpu->registers().set_subtraction_flag(false);
    cpu->registers().set_zero_flag(result == 0);
    cpu->registers().set_half_carry_flag(false);
    cpu->registers().set_carry_flag(value & 1);

    operand = result;
  }
};

struct SWAP {
  template <typename Bus, typename Operand>
  void operator()(CPU<Bus>* cpu) {
    auto operand = Operand::template get<Bus>(cpu);
    VERBOSE_PRINT() << "SWAP: " << operand.address_str() << std::endl;
    uint8_t value = operand.get();
    uint8_t result = (value << 4) | (value >> 4);
    if constexpr (std::is_same_v<Operand, typename ::Operand::HLMEM>)
      cpu->tick();
    cpu->registers().set_subtraction_flag(false);
    cpu->registers().set_zero_flag(result == 0);
    cpu->registers().set_half_carry_flag(false);
    cpu->registers().set_carry_flag(false);
    operand = result;
  }
};

struct BIT {
  template <typename Bus, typename BitOperand, typename TargetOperand>
  void operator()(CPU<Bus>* cpu) {
    uint8_t bit = BitOperand::template get<Bus>(cpu);
    auto target = TargetOperand::template get<Bus>(cpu);
    VERBOSE_PRINT() << "BIT: test bit " << static_cast<int>(bit) << " in " << target.address_str()
                    << std::endl;
    cpu->registers().set_zero_flag((target.get() & (1 << bit)) == 0);
    cpu->registers().set_half_carry_flag(true);
    cpu->registers().set_subtraction_flag(false);
  }
};

struct RES {
  template <typename Bus, typename BitOperand, typename TargetOperand>
  void operator()(CPU<Bus>* cpu) {
    uint8_t bit = BitOperand::template get<Bus>(cpu);
    auto target = TargetOperand::template get<Bus>(cpu);
    auto value = target.get();

    if constexpr (std::is_same_v<TargetOperand, typename ::Operand::HLMEM>)
      cpu->tick();

    VERBOSE_PRINT() << "RES: reset bit " << static_cast<int>(bit) << " in " << target.address_str()
                    << std::endl;
    target = value & ~(1 << bit);
  }
};

struct SET {
  template <typename Bus, typename BitOperand, typename TargetOperand>
  void operator()(CPU<Bus>* cpu) {
    uint8_t bit = BitOperand::template get<Bus>(cpu);
    auto target = TargetOperand::template get<Bus>(cpu);
    auto value = target.get();
    if constexpr (std::is_same_v<TargetOperand, typename ::Operand::HLMEM>)
      cpu->tick();
    VERBOSE_PRINT() << "SET: set bit " << static_cast<int>(bit) << " in " << target.address_str()
                    << std::endl;
    target = value | (1 << bit);
  }
};

struct JP {
  template <typename Bus, typename Operand>
  void operator()(CPU<Bus>* cpu) {
    auto address = Operand::template get<Bus>(cpu).get();
    VERBOSE_PRINT() << "JP: PC = " << std::hex << address << std::dec << std::endl;
    if constexpr (std::is_same_v<Operand, typename ::Operand::IMM16>)
      cpu->tick();
    cpu->pc().set(address);
  }
};

struct JP_COND {
  template <typename Bus, typename CondOperand, typename AddrOperand>
  void operator()(CPU<Bus>* cpu) {
    const bool condition = CondOperand::template get<Bus>(cpu);
    auto addr = AddrOperand::template get<Bus>(cpu);
    VERBOSE_PRINT() << " JP_COND: condition " << condition << ", offset "
                    << static_cast<int32_t>(static_cast<int8_t>(addr.get())) << std::endl;
    if (condition) {
      cpu->pc().set(addr.get());
      cpu->tick();
    }
  }
};

struct JR {
  template <typename Bus, typename Operand>
  void operator()(CPU<Bus>* cpu) {
    const int8_t offset = static_cast<int8_t>(Operand::template get<Bus>(cpu).get());
    VERBOSE_PRINT() << "JR: PC += " << static_cast<int32_t>(offset) + 1 << std::endl;
    cpu->tick();
    cpu->pc().set(static_cast<int32_t>(cpu->pc().get()) + offset);
  }
};

struct JR_COND {
  template <typename Bus, typename CondOperand, typename OffsetOperand>
  void operator()(CPU<Bus>* cpu) {
    bool condition = CondOperand::template get<Bus>(cpu);
    const int8_t offset = static_cast<int8_t>(OffsetOperand::template get<Bus>(cpu).get());
    VERBOSE_PRINT() << " JR_COND: condition " << condition << ", offset " << static_cast<int32_t>(offset) + 1
                    << std::endl;
    if (condition) {
      cpu->pc().set(static_cast<int32_t>(cpu->pc().get()) + offset);
      cpu->tick();
    }
  }
};

struct CALL {
  template <typename Bus, typename Operand>
  void operator()(CPU<Bus>* cpu) {
    auto addr = Operand::template get<Bus>(cpu);
    VERBOSE_PRINT() << "CALL: address " << std::hex << addr << std::dec << std::endl;
    cpu->tick();
    cpu->stack().push_16(cpu->pc().get());
    cpu->tick();
    cpu->tick();
    cpu->pc().set(addr.get());
  }
};

struct RST {
  template <typename Bus, typename Operand>
  void operator()(CPU<Bus>* cpu) {
    cpu->tick();
    cpu->tick();
    auto addr = Operand::template get<Bus>(cpu);
    VERBOSE_PRINT() << "RST: address " << std::hex << addr << std::dec << std::endl;

    cpu->stack().push_8(cpu->pc().get() >> 8);
    cpu->tick();
    cpu->stack().push_8(cpu->pc().get() & 0xFF);

    cpu->pc().set(addr.get());
  }
};

struct CALL_COND {
  template <typename Bus, typename CondOperand, typename AddrOperand>
  void operator()(CPU<Bus>* cpu) {
    bool condition = CondOperand::template get<Bus>(cpu);
    auto addr = AddrOperand::template get<Bus>(cpu);
    VERBOSE_PRINT() << "CALL_COND: condition " << condition << ", address " << addr << std::endl;
    if (condition) {
      cpu->tick();
      cpu->stack().push_16(cpu->pc().get());
      cpu->tick();
      cpu->tick();
      cpu->pc().set(addr.get());
    }
  }
};

struct RET {
  template <typename Bus>
  void operator()(CPU<Bus>* cpu) {
    cpu->tick();
    uint16_t address_low = cpu->stack().pop_8();
    cpu->tick();
    uint16_t address_high = cpu->stack().pop_8();
    cpu->tick();
    uint16_t address = (address_high << 8) | address_low;
    VERBOSE_PRINT() << "RET to " << std::hex << address << std::dec << std::endl;

    cpu->pc().set(address);
  }
};

struct RET_COND {
  template <typename Bus, typename CondOperand>
  void operator()(CPU<Bus>* cpu) {
    cpu->tick();
    VERBOSE_PRINT() << "RET_COND: condition ";
    bool condition = CondOperand::template get<Bus>(cpu);
    VERBOSE_PRINT() << " = " << condition << std::endl;

    if (condition) {
      cpu->tick();
      uint16_t address_low = cpu->stack().pop_8();
      cpu->tick();
      uint16_t address_high = cpu->stack().pop_8();
      cpu->tick();
      uint16_t address = (address_high << 8) | address_low;
      cpu->pc().set(address);
    }
  }
};

struct RETI {
  template <typename Bus>
  void operator()(CPU<Bus>* cpu) {

    cpu->tick();
    uint16_t address_low = cpu->stack().pop_8();
    cpu->tick();
    uint16_t address_high = cpu->stack().pop_8();
    cpu->enable_interrupts();
    cpu->tick();
    uint16_t address = (address_high << 8) | address_low;
    VERBOSE_PRINT() << "RETI to " << std::hex << address << std::dec << std::endl;
    cpu->pc().set(address);
  }
};
};  // namespace Operator