#pragma once
#include <sstream>
#include <string>
#include <type_traits>

namespace StringUtils {

inline std::string binary(uint8_t a) {
  std::stringstream s;
  for (int b = 7; b >= 0; --b) {
    s << ((a >> b) & 1);
  }
  return s.str();
}

template <typename T>
inline std::string hex(T a) {
  static_assert(sizeof(T) <= 4, "Type size must be <= 4 bytes");
  std::stringstream s;
  if constexpr (std::is_signed_v<T>) {
    s << std::hex << "0x" << static_cast<int64_t>(a);
  } else {
    s << std::hex << "0x" << static_cast<uint64_t>(a);
  }
  return s.str();
}

inline std::string oct(uint8_t a) {
  std::stringstream s;
  s << std::oct << static_cast<unsigned int>(a);
  return s.str();
}

}  // namespace StringUtils
