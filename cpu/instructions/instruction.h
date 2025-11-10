#pragma once

#include <inttypes.h>
#include <tuple>

template <typename T, typename... U>
class Instruction {
public:
  using Operator = T;
  using Operands = std::tuple<U...>;
};