#pragma once

#include <cstdint>
#include "constants.h"

namespace FlagOps {
struct NONE;
struct SET;
struct ADD8FLAGS;
struct ADD16FLAGS;
struct ADDSPIMM8;

inline bool check_zero_flag_inc(uint8_t a) {
  return a == BYTE_MASK;
}

inline bool check_zero_flag_dec(uint8_t a) {
  return a == 1;
}

// Example for INC r8 (addition with 1)
inline bool check_half_carry_inc(uint8_t r8) {
  return (r8 & LOWER_NIBBLE_MASK) == LOWER_NIBBLE_MASK;
}

inline bool check_half_carry_dec(uint8_t r8) {
  return (r8 & LOWER_NIBBLE_MASK) == 0x00;
}

inline bool check_half_carry_sub(uint8_t a, uint8_t b) {
  // Check if there's a borrow from bit 4 to bit 3
  // This happens when the lower nibble of b is greater than the lower nibble of a
  return (a & LOWER_NIBBLE_MASK) < (b & LOWER_NIBBLE_MASK);
}

inline bool check_carry_sub(uint8_t a, uint8_t b) {
  // Check if there's a borrow (underflow)
  // This happens when b is greater than a
  return a < b;
}

inline bool check_half_carry_add8(uint8_t a, uint8_t b) {
  return (a & LOWER_NIBBLE_MASK) + (b & LOWER_NIBBLE_MASK) > LOWER_NIBBLE_MASK;
}

inline bool check_half_carry_add8(uint8_t a, uint8_t b, uint8_t carry) {
  return (a & LOWER_NIBBLE_MASK) + (b & LOWER_NIBBLE_MASK) + carry > LOWER_NIBBLE_MASK;
}

inline bool check_carry_add8(uint8_t a, uint8_t b) {
  return static_cast<uint16_t>(a) + static_cast<uint16_t>(b) > BYTE_MASK;
}

inline bool check_carry_add8(uint8_t a, uint8_t b, uint8_t carry) {
  return static_cast<uint16_t>(a) + static_cast<uint16_t>(b) + static_cast<uint16_t>(carry) > BYTE_MASK;
}

inline bool check_half_carry_add16(uint16_t a, uint16_t b) {
  return (a & LOWER_12_BITS_MASK) + (b & LOWER_12_BITS_MASK) > LOWER_12_BITS_MASK;
}

inline bool check_carry_add16(uint16_t a, uint16_t b) {
  return static_cast<uint32_t>(a) + static_cast<uint32_t>(b) > WORD_MASK;
}

inline bool check_carry_addsp(uint16_t a, int8_t b) {
  if (b >= 0) {
    return ((a & BYTE_MASK) + b) > BYTE_MASK;
  } else {
    return ((a + b) & BYTE_MASK) <= (a & BYTE_MASK);
  }
}

inline bool check_half_carry_addsp(uint16_t a, int8_t b) {
  if (b >= 0) {
    return (a & LOWER_NIBBLE_MASK) + (b & LOWER_NIBBLE_MASK) > LOWER_NIBBLE_MASK;
  } else {
    return ((a + b) & LOWER_NIBBLE_MASK) <= (a & LOWER_NIBBLE_MASK);
  }
}
}  // namespace FlagOps