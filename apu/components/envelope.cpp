#include "envelope.h"

// Maximum envelope volume
constexpr uint8_t MAX_VOLUME = 0x0F;

Envelope::Envelope() = default;

void Envelope::trigger(uint8_t volume, bool direction_up, uint8_t initial_period) {
  volume_ = volume;
  direction_up_ = direction_up;
  initial_period_ = initial_period;
  period_ = initial_period;
}

void Envelope::tick() {
  if (initial_period_ == 0)
    return;

  period_--;

  if (period_ == 0) {
    period_ = initial_period_;
    if (volume_ < MAX_VOLUME && direction_up_) {
      volume_++;
    } else if (volume_ > 0 && !direction_up_) {
      volume_--;
    }
  }
}

uint8_t Envelope::output_volume() const {
  return volume_;
}
