#pragma once

#include <inttypes.h>
#include "constants.h"
#include "memory_location.h"

// ===== Initial Register Values (DMG Mode) =====
namespace InitialRegisterValues {
constexpr uint16_t AF = 0x01B0;  // DMG; CGB = 0x11B0
constexpr uint16_t BC = 0x0013;
constexpr uint16_t DE = 0x00D8;
constexpr uint16_t HL = 0x014D;
constexpr uint16_t SP = 0xFFFE;
}  // namespace InitialRegisterValues

struct register16 {
  register16() = default;
  register16(const uint16_t initial_value) : value16(initial_value) {}

  union {
    uint16_t value16 = 0;
    struct {
      uint8_t low = 0;
      uint8_t high = 0;
    } value8;
  };
};

class CPURegisters {
public:
  CPURegisters(bool boot_rom_active) {
    if (!boot_rom_active) {
      AF_ = InitialRegisterValues::AF;
      BC_ = InitialRegisterValues::BC;
      DE_ = InitialRegisterValues::DE;
      HL_ = InitialRegisterValues::HL;
      pc_ = ROM_START;
      sp_ = InitialRegisterValues::SP;
    }
  }

  [[gnu::always_inline]] inline RegisterLocation16 pc() { return RegisterLocation16{pc_, "PC"}; }
  [[gnu::always_inline]] inline RegisterLocation8 A() { return RegisterLocation8{AF_.value8.high, "A"}; }
  [[gnu::always_inline]] inline RegisterLocation8 F() { return RegisterLocation8{AF_.value8.low, "F"}; }
  [[gnu::always_inline]] inline RegisterLocation8 B() { return RegisterLocation8{BC_.value8.high, "B"}; }
  [[gnu::always_inline]] inline RegisterLocation8 C() { return RegisterLocation8{BC_.value8.low, "C"}; }
  [[gnu::always_inline]] inline RegisterLocation8 D() { return RegisterLocation8{DE_.value8.high, "D"}; }
  [[gnu::always_inline]] inline RegisterLocation8 E() { return RegisterLocation8{DE_.value8.low, "E"}; }
  [[gnu::always_inline]] inline RegisterLocation8 H() { return RegisterLocation8{HL_.value8.high, "H"}; }
  [[gnu::always_inline]] inline RegisterLocation8 L() { return RegisterLocation8{HL_.value8.low, "L"}; }

  [[gnu::always_inline]] inline RegisterLocation16 AF() { return RegisterLocation16{AF_.value16, "AF"}; }
  [[gnu::always_inline]] inline RegisterLocation16 BC() { return RegisterLocation16{BC_.value16, "BC"}; }
  [[gnu::always_inline]] inline RegisterLocation16 DE() { return RegisterLocation16{DE_.value16, "DE"}; }
  [[gnu::always_inline]] inline RegisterLocation16 HL() { return RegisterLocation16{HL_.value16, "HL"}; }

  [[gnu::always_inline]] inline RegisterLocation16 SP() { return RegisterLocation16{sp_, "SP"}; }

  [[gnu::always_inline]] inline bool get_zero_flag() const { return AF_.value8.low & ZERO_FLAG_BIT; }
  [[gnu::always_inline]] inline void set_zero_flag(const bool value) {
    if (value)
      AF_.value8.low |= ZERO_FLAG_BIT;
    else
      AF_.value8.low &= ~ZERO_FLAG_BIT;
  }

  [[gnu::always_inline]] inline bool get_subtraction_flag() const {
    return AF_.value8.low & SUBTRACTION_FLAG_BIT;
  }
  [[gnu::always_inline]] inline void set_subtraction_flag(const bool value) {
    if (value)
      AF_.value8.low |= SUBTRACTION_FLAG_BIT;
    else
      AF_.value8.low &= ~SUBTRACTION_FLAG_BIT;
  }

  [[gnu::always_inline]] inline bool get_half_carry_flag() const {
    return AF_.value8.low & HALF_CARRY_FLAG_BIT;
  }
  [[gnu::always_inline]] inline void set_half_carry_flag(const bool value) {
    if (value)
      AF_.value8.low |= HALF_CARRY_FLAG_BIT;
    else
      AF_.value8.low &= ~HALF_CARRY_FLAG_BIT;
  }

  [[gnu::always_inline]] inline bool get_carry_flag() const { return AF_.value8.low & CARRY_FLAG_BIT; }
  [[gnu::always_inline]] inline void set_carry_flag(const bool value) {
    if (value)
      AF_.value8.low |= CARRY_FLAG_BIT;
    else
      AF_.value8.low &= ~CARRY_FLAG_BIT;
  }

private:
  register16 AF_ = 0x0000;  //DMG
  register16 BC_ = 0x0000;
  register16 DE_ = 0x0000;
  register16 HL_ = 0x0000;
  uint16_t pc_ = 0x0000;
  uint16_t sp_ = 0x0000;
};
