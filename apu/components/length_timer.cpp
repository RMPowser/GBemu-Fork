#include "length_timer.h"
#include "audio_constants.h"
#include "frame_sequencer.h"

template <uint32_t BASE_LENGTH>
LengthTimerInternal<BASE_LENGTH>::LengthTimerInternal(FrameSequencer& frame_sequencer,
                                                      std::function<void()> length_expired_callback)
    : frame_sequencer_(frame_sequencer), length_expired_callback_(length_expired_callback) {}

template <uint32_t BASE_LENGTH>
void LengthTimerInternal<BASE_LENGTH>::enable(bool enabled) {
  bool previous_enabled_ = enabled_;
  enabled_ = enabled;
  if (enabled) {
    start();
    if (!previous_enabled_ && !frame_sequencer_.next_step_is_length_counter()) {
      tick();
    }
  } else {
    should_play_ = CHANNEL_ENABLED;
  }
}

template <uint32_t BASE_LENGTH>
void LengthTimerInternal<BASE_LENGTH>::trigger() {
  if (timer_ == 0) {
    timer_ = BASE_LENGTH;
    if (!frame_sequencer_.next_step_is_length_counter()) {
      tick();
    }
  }
}

template <uint32_t BASE_LENGTH>
void LengthTimerInternal<BASE_LENGTH>::start() {
  should_play_ = CHANNEL_ENABLED;
}

template <uint32_t BASE_LENGTH>
void LengthTimerInternal<BASE_LENGTH>::set_length(uint16_t length_register) {
  length_register_ = length_register;
  timer_ = BASE_LENGTH - length_register;
}

template class LengthTimerInternal<64>;
template class LengthTimerInternal<256>;