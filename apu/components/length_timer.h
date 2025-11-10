#pragma once

#include <cstdint>
#include <functional>
#include "audio_constants.h"

class FrameSequencer;

template <uint32_t BASE_LENGTH>
class LengthTimerInternal {
public:
  LengthTimerInternal(FrameSequencer& frame_sequencer, std::function<void()> length_expired_callback);

  [[gnu::always_inline]] void tick() {
    if (timer_ == 0 || !enabled_) {
      return;
    }

    timer_--;

    if (timer_ == 0) {
      length_expired_callback_();
      should_play_ = CHANNEL_DISABLED;
    }
  }

  void set_length(uint16_t length_register);
  void enable(bool enabled);

  void trigger();

  [[gnu::always_inline]] float should_play() const { return should_play_; }

private:
  void start();

  bool enabled_ = false;
  float should_play_ = CHANNEL_DISABLED;
  uint16_t timer_ = 0;
  uint16_t length_register_ = 0;
  FrameSequencer& frame_sequencer_;
  std::function<void()> length_expired_callback_;
};

using LengthTimer = LengthTimerInternal<64>;
using LengthTimerChannel3 = LengthTimerInternal<256>;