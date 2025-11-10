#pragma once

#include <cstdint>

class Envelope {
public:
  Envelope();

  void trigger(uint8_t volume, bool direction_up, uint8_t initial_period);
  void tick();
  uint8_t output_volume() const;

private:
  uint8_t volume_ = 0;
  uint8_t initial_period_ = 0;
  uint8_t period_ = 0;
  bool direction_up_ = false;
};
