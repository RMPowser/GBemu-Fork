#pragma once

#include <inttypes.h>
#include <functional>

struct JoypadState;

struct OSBridge {
  std::function<void(const int16_t* samples, int num_samples)> on_audio_generated;
  std::function<void()> present_frame;
  std::function<bool(JoypadState& joypad_state)> handle_events;
  std::function<void(const uint32_t* pixels, size_t pitch)> blit_screen;
};