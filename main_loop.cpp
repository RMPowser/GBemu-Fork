#include "main_loop.h"
#include <chrono>
#include <iostream>
#include "OSBridge.h"
#include "joypad_state.h"
#include "ppu_bridge.h"
#include "rom_loader.h"

using namespace std::chrono;

// ===== Main Loop Timing Constants =====
namespace {
constexpr std::chrono::microseconds TARGET_FRAME_DURATION_MICROSECONDS(
    16740);                                         // Game Boy runs at 59.73 Hz (16.74ms)
constexpr uint32_t FPS_MEASUREMENT_INTERVAL = 300;  // Measure FPS every 300 frames
}  // namespace

MainLoop::MainLoop(ROMLoader& loader, OSBridge& os_bridge)
    : cpu_(loader, ppu_, apu_, bus_),
      ppu_bridge_({[&]() { cpu_.hardware_registers().trigger_vblank_interrupt(); },
                   [&]() { cpu_.hardware_registers().trigger_lcd_stat_interrupt(); }, os_bridge.blit_screen,
                   [&]() { return cpu_.is_halted(); },
                   [&](uint16_t address) -> const uint8_t* { return cpu_.memory_bridge().read(address); }}),
      ppu_(ppu_bridge_, loader.has_boot_rom()),
      apu_(os_bridge.on_audio_generated),
      os_bridge_(os_bridge) {}

bool MainLoop::run(JoypadState& joypad_state) {
  cpu_.update_joypad_state(joypad_state);
  cpu_.run_single_instruction();

  if (ppu_.frame_completed()) {
    apu_.generate_samples();

    auto current_time = steady_clock::now();
    busy_wait(current_time);
    os_bridge_.present_frame();
    frame_count_++;

    if (frame_count_ == FPS_MEASUREMENT_INTERVAL)
      calculate_fps();

    return true;
  }

  return false;
}

void MainLoop::run_once() {
  cpu_.run_single_instruction();
}

CPU<Bus>& MainLoop::cpu() {
  return cpu_;
}

void MainLoop::calculate_fps() {
  auto current_time = steady_clock::now();
  auto total_elapsed_time = current_time - last_fps_time_;
  auto actual_fps =
      static_cast<double>(frame_count_) / duration_cast<duration<double>>(total_elapsed_time).count();

  // Calculate theoretical FPS without sleep limiting
  auto actual_render_time = total_elapsed_time - total_sleep_time_;
  auto time_per_frame = actual_render_time / FPS_MEASUREMENT_INTERVAL;

  auto theoretical_fps = std::chrono::seconds(1) / time_per_frame;

  std::cout << "FPS: " << actual_fps << " (Actual: " << theoretical_fps << ")" << std::endl;

  frame_count_ = 0;
  last_fps_time_ = current_time;

  total_sleep_time_ = microseconds(0);  // Reset sleep time for next measurement period
}

void MainLoop::busy_wait(time_point<steady_clock> current_time) {
  auto frame_elapsed = current_time - last_present_time_;

  if (frame_elapsed < TARGET_FRAME_DURATION_MICROSECONDS) {
    auto sleep_duration = TARGET_FRAME_DURATION_MICROSECONDS - frame_elapsed;
    auto end = current_time + sleep_duration;
    total_sleep_time_ += duration_cast<microseconds>(sleep_duration);

    while (steady_clock::now() < end) {}
    last_present_time_ += TARGET_FRAME_DURATION_MICROSECONDS;
  } else {
    last_present_time_ = current_time;
  }
}

void MainLoop::serialize(SaveStateSerializer& serializer) const {
  serializer << cpu_;
  serializer << apu_;
  serializer << ppu_;
}

void MainLoop::deserialize(SaveStateSerializer& serializer) {
  serializer >> cpu_;
  serializer >> apu_;
  serializer >> ppu_;
}
