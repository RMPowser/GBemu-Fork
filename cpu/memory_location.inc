#pragma once

#include "memory_location.h"
#include "utils.h"

template <typename T>
inline RegisterLocation<T>& RegisterLocation<T>::operator=(const RegisterLocation<T>& other) {
  reg_ = other.reg_;
  return *this;
}

template <typename T>
template <typename Bus>
inline RegisterLocation<T>& RegisterLocation<T>::operator=(const MemoryLocation<Bus>& other) {
  if (sizeof(T) == 2) {
    FATAL("This doesn't work, register location is 16 bits");
  }

  reg_ = other.get();
  return *this;
}

template <typename T>
inline RegisterLocation<T>& RegisterLocation<T>::operator=(const T other) {
  reg_ = other;
  return *this;
}

template <typename T>
template <typename U>
inline RegisterLocation<T>& RegisterLocation<T>::operator=(const ROMLocation<U>& other) {
  reg_ = other.get();
  return *this;
}

template <typename Bus>
inline MemoryLocation<Bus>& MemoryLocation<Bus>::operator=(const MemoryLocation<Bus>& other) {
  memory_bridge_.write(address_, other.get());
  return *this;
}

template <typename Bus>
template <typename T>
inline MemoryLocation<Bus>& MemoryLocation<Bus>::operator=(const RegisterLocation<T>& other) {
  if constexpr (std::is_same_v<T, uint16_t>) {
    memory_bridge_.write(address_, other.get() & 0xFF);
    memory_bridge_.write(address_ + 1, other.get() >> 8);
  } else {
    memory_bridge_.write(address_, other.get());
  }
  return *this;
}

template <typename Bus>
template <typename T>
inline MemoryLocation<Bus>& MemoryLocation<Bus>::operator=(const ROMLocation<T>& other) {
  memory_bridge_.write(address_, other.get());
  return *this;
}

template <typename Bus>
inline MemoryLocation<Bus>& MemoryLocation<Bus>::operator=(const unsigned char other) {
  memory_bridge_.write(address_, other);
  return *this;
}

template <typename Bus>
inline const unsigned char MemoryLocation<Bus>::get() const {
  return *memory_bridge_.read(address_);
}