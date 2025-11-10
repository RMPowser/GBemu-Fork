#pragma once

class PPU;
class APU;
class Timer;
class HardwareRegisters;
class Joypad;
class MemoryController;

template <typename Bus>
class CPU;

struct Bus {
  PPU* ppu_;
  APU* apu_;
  CPU<Bus>* cpu_;
  Timer* timer_;
  HardwareRegisters* hw_registers_;
  MemoryController* memory_controller_;
  Joypad* joypad_;
};