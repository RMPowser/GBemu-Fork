#pragma once

#include <cstdint>
#include "length_timer.h"
#include "stack_vector.h"

class Envelope;

class Sweep;

class FrameSequencer {
public:
  FrameSequencer() = default;
  void tick();
  void reset();

  bool next_step_is_length_counter() const;

  void add_envelope(Envelope* envelope);

  template <typename T>
  void add_length_timer(T* length_timer) {
    if constexpr (std::is_same_v<T, LengthTimer>) {
      length_counters_.push_back(length_timer);
    } else if constexpr (std::is_same_v<T, LengthTimerChannel3>) {
      length_counter_channel_3_.push_back(length_timer);
    }
  }
  void add_sweep(Sweep* sweep);

private:
  template <typename T>
  void run_step(const T& v);
  uint8_t step_ = 0;

  StackVector<LengthTimer*, 3> length_counters_;
  StackVector<LengthTimerChannel3*, 1> length_counter_channel_3_;
  StackVector<Envelope*, 3> envelopes_;
  StackVector<Sweep*, 1> sweeps_;
};
