#pragma once

#include <inttypes.h>
#include <algorithm>
#include <array>
#include <ostream>
#include <string>

#include "constants.h"

class MemoryController;

template <typename Bus>
class FirstLevelMemoryBridge;

template <typename Bus>
class MemoryLocation;

template <typename T>
class ROMLocation;

template <typename T>
class RegisterLocation {
public:
  RegisterLocation(T& reg, const char* name) : reg_(reg) {
    if constexpr (VERBOSE)
      std::copy_n(name, sizeof(T), name_.begin());
  }
  RegisterLocation<T>& operator=(const RegisterLocation<T>& other);
  template <typename Bus>
  RegisterLocation<T>& operator=(const MemoryLocation<Bus>& other);
  RegisterLocation<T>& operator=(const T other);
  template <typename U>
  RegisterLocation<T>& operator=(const ROMLocation<U>& other);

  friend std::ostream& operator<<(std::ostream& os, const RegisterLocation<T>& reg_loc) {
    os << reg_loc.address_str() << " = " << +reg_loc.get();
    return os;
  }

  T get() const { return reg_; }

  std::string address_str() const { return std::string("Register: ") + std::string(name_.data(), sizeof(T)); }

private:
  std::array<char, 2> name_;
  T& reg_;
};

template <typename Bus>
class MemoryLocation {
public:
  MemoryLocation(uint16_t address, MemoryController& mc, FirstLevelMemoryBridge<Bus>& memory_bridge)
      : address_(address), mc_(mc), memory_bridge_(memory_bridge) {}
  MemoryLocation& operator=(const MemoryLocation& other);
  template <typename T>
  MemoryLocation& operator=(const RegisterLocation<T>& other);
  template <typename T>
  MemoryLocation& operator=(const ROMLocation<T>& other);
  MemoryLocation& operator=(const unsigned char other);
  friend std::ostream& operator<<(std::ostream& os, const MemoryLocation& mem_loc) {
    os << mem_loc.address_str() << " = " << +mem_loc.get();
    return os;
  }

  const unsigned char get() const;
  std::string address_str() const {
    char buf[8];
    snprintf(buf, sizeof(buf), "%04X", address_);
    auto hex_addr = std::string("0x") + buf;

    return std::string("Mem: ") + std::to_string(address_) + " " + hex_addr;
  }

private:
  uint16_t address_;
  MemoryController& mc_;
  FirstLevelMemoryBridge<Bus>& memory_bridge_;
};

template <typename T>
class ROMLocation {
public:
  ROMLocation(T data) : data_(data) {}

  friend std::ostream& operator<<(std::ostream& os, const ROMLocation<T>& reg_loc) {
    os << (int)reg_loc.get();
    return os;
  }
  const T get() const { return data_; }
  T data_;
};

using RegisterLocation8 = RegisterLocation<uint8_t>;
using RegisterLocation16 = RegisterLocation<uint16_t>;
using ROMLocation8 = ROMLocation<uint8_t>;
using ROMLocation16 = ROMLocation<uint16_t>;
#include "memory_location.inc"