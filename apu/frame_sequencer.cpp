#include "frame_sequencer.h"
#include "envelope.h"
#include "length_timer.h"
#include "sweep.h"

void FrameSequencer::reset() {
  step_ = 0;
}

void FrameSequencer::tick() {
  switch (step_) {
    case 0:
      run_step(length_counters_);
      run_step(length_counter_channel_3_);
      break;
    case 2:
      run_step(length_counters_);
      run_step(length_counter_channel_3_);
      run_step(sweeps_);
      break;
    case 4:
      run_step(length_counters_);
      run_step(length_counter_channel_3_);
      break;
    case 6:
      run_step(length_counters_);
      run_step(length_counter_channel_3_);
      run_step(sweeps_);
      break;
    case 7:
      run_step(envelopes_);
      break;
  }
  step_ = (step_ + 1) % 8;
}

template <typename T>
void FrameSequencer::run_step(const T& v) {
  for (auto& item : v) {
    item->tick();
  }
}

void FrameSequencer::add_envelope(Envelope* envelope) {
  envelopes_.push_back(envelope);
}

void FrameSequencer::add_sweep(Sweep* sweep) {
  sweeps_.push_back(sweep);
}

bool FrameSequencer::next_step_is_length_counter() const {
  return step_ % 2 == 0;
}