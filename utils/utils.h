#pragma once

#include <iostream>     // IWYU pragma: keep
#include "constants.h"  // IWYU pragma: keep

#define FATAL(x)                 \
  do {                           \
    std::cout << x << std::endl; \
    std::exit(-1);               \
  } while (0)

#define VERBOSE_PRINT()  \
  if constexpr (VERBOSE) \
  std::cout
