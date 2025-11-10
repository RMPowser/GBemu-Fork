#pragma once

#include <inttypes.h>
#include <chrono>
#include "OSBridge.h"
#include "apu.h"
#include "bus.h"
#include "cpu.h"
#include "ppu.h"
#include "ppu_bridge.h"

class ROMLoader;

class MainLoop {
public:
  MainLoop(ROMLoader& loader, OSBridge& bridge);
  bool run(JoypadState& joypad_state);
  void run_once();
  CPU<Bus>& cpu();

  void serialize(SaveStateSerializer& serializer) const;
  void deserialize(SaveStateSerializer& serializer);

private:
  void busy_wait(std::chrono::time_point<std::chrono::steady_clock> current_time);
  void calculate_fps();

  CPU<Bus> cpu_;
  PPUBridge ppu_bridge_;
  PPU ppu_;
  APU apu_;
  Bus bus_;
  std::chrono::steady_clock::time_point last_present_time_ = std::chrono::steady_clock::now();
  std::chrono::steady_clock::time_point last_fps_time_ = std::chrono::steady_clock::now();
  uint32_t frame_count_ = 0;
  std::chrono::microseconds total_sleep_time_ = std::chrono::microseconds(0);
  OSBridge os_bridge_;
};
