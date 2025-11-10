#pragma once

#include <cstdint>
#include "LFSR.h"
#include "dac.h"
#include "length_timer.h"
#include "wave_duty.h"

struct Channel1Traits {
  static constexpr bool HAS_SWEEP = true;
  static constexpr uint16_t SweepRegister = 0xFF10;          // Sweep
  static constexpr uint16_t LengthRegister = 0xFF11;         // Length/Duty
  static constexpr uint16_t EnvelopeRegister = 0xFF12;       // Envelope
  static constexpr uint16_t FrequencyLowRegister = 0xFF13;   // Frequency Low
  static constexpr uint16_t FrequencyHighRegister = 0xFF14;  // Frequency High/Control
  static constexpr uint8_t PANNING_LEFT_BIT = 4;             // NR51 bit 4
  static constexpr uint8_t PANNING_RIGHT_BIT = 0;            // NR51 bit 0
  static constexpr uint16_t NR52_BIT = 0;                    // NR52 bit 0
  using LengthTimerType = LengthTimer;
  using DACType = DAC<WaveDuty>;
};

struct Channel2Traits {
  static constexpr bool HAS_SWEEP = false;
  static constexpr uint16_t LengthRegister = 0xFF16;         // Length/Duty
  static constexpr uint16_t EnvelopeRegister = 0xFF17;       // Envelope
  static constexpr uint16_t FrequencyLowRegister = 0xFF18;   // Frequency Low
  static constexpr uint16_t FrequencyHighRegister = 0xFF19;  // Frequency High/Control
  static constexpr uint8_t PANNING_LEFT_BIT = 5;             // NR51 bit 5
  static constexpr uint8_t PANNING_RIGHT_BIT = 1;            // NR51 bit 1
  static constexpr uint16_t NR52_BIT = 1;                    // NR52 bit 1
  using LengthTimerType = LengthTimer;
  using DACType = DAC<WaveDuty>;
};

struct Channel3Traits {
  static constexpr uint16_t NR52_BIT = 2;  // NR52 bit 2
  using LengthTimerType = LengthTimerChannel3;
  using DACType = WaveRAMDAC;
};

struct Channel4Traits {
  static constexpr uint16_t NR52_BIT = 3;  // NR52 bit 3
  using LengthTimerType = LengthTimer;
  using DACType = DAC<LFSR>;
};