# APU Library

A self-contained Game Boy Audio Processing Unit (APU) library with zero external dependencies. This library handles all audio generation for a Game Boy emulator.

## Quick Start

### Basic Usage

```cpp
#include "apu/apu.h"

// Create APU with sample generation callback
APU apu([](const int16_t* samples, int num_samples) {
    // Send samples to audio output
    audio_device.queue_samples(samples, num_samples);
});
```

## API Reference

### Constructor

```cpp
APU(std::function<void(const int16_t* samples, int num_samples)> sample_generated_callback);
```

Creates a new APU instance. The callback function is called whenever audio samples are generated:

- `samples`: Pointer to stereo interleaved 16-bit signed samples (L, R, L, R, ...)
- `num_samples`: Number of sample pairs (stereo = 2 * num_samples total samples)

### Main Methods

#### `void tick()`
Call this method **once per M-cycle** (once per CPU instruction). This advances the APU state machine and generates audio samples.

#### `void tick_frame_sequencer()`
Call this method **every 2048 M-cycles** (or more specifically, when the 10th bit falls on the internal clock). This handles length counters, envelope, and sweep operations.

**Example:**
```cpp
// Check bit 10 of APU clock
if ((internal_clock & 0x400) && !(internal_clock & 0x400)) {
    apu.tick_frame_sequencer();
}
```

#### `void generate_samples()`
This doesn't need to be called, but should be called just before pausing for FPS management to clear the sample buffer. This ensures all generated samples are flushed to the callback.

### Register Access

#### Audio Register Access
```cpp
void audio_register_write(uint16_t address, uint8_t value);
const unsigned char* audio_register_read(uint16_t address) const;
```
- `address`: Register address (0xFF10-0xFF3F)
- Handles all audio registers including:
  - **0xFF10-0xFF14**: Channel 1 (Square with sweep)
  - **0xFF16-0xFF19**: Channel 2 (Square)
  - **0xFF1A-0xFF1E**: Channel 3 (Wave)
  - **0xFF20-0xFF23**: Channel 4 (Noise)
  - **0xFF24**: NR50 (Master volume and panning)
  - **0xFF25**: NR51 (Sound panning)
  - **0xFF26**: NR52 (Sound on/off)
  - **0xFF30-0xFF3F**: Wave RAM

## Integration Example

Here's a complete example of integrating the APU into your emulator:

```cpp
#include "apu/apu.h"

class MyEmulator {
    APU apu_;
    uint32_t apu_clock_ = 0;
    uint32_t previous_apu_clock_ = 0;
    
    void on_samples_generated(const int16_t* samples, int num_samples) {
        // Send samples to audio output
        audio_device_.queue_samples(samples, num_samples);
    }
    
public:
    MyEmulator() 
        : apu_([this](const int16_t* samples, int num_samples) {
            on_samples_generated(samples, num_samples);
          })
    {}
    
    //Called once per m-cycle
    void tick() {
        // Tick APU once per M-cycle
        apu_.tick();
        
        // Update APU clock and check for frame sequencer tick
        previous_clock_ = clock_;
        clock_++;
        
        // Tick frame sequencer when bit 10 transitions from 0 to 1
        if ((clock_ & 0x400) && !(previous_clock_ & 0x400)) {
            apu_.tick_frame_sequencer();
        }
        
        // Optionally generate samples periodically
        static uint32_t sample_counter = 0;
        if (++sample_counter >= 100) {  // Adjust based on your needs
            apu_.generate_samples();
            sample_counter = 0;
        }
    }
    
    // Route memory accesses to APU
    void handle_memory_write(uint16_t addr, uint8_t value) {
        if (addr >= 0xFF10 && addr <= 0xFF3F) {
            apu_.audio_register_write(addr, value);
        }
    }
    
    const uint8_t* handle_memory_read(uint16_t addr) {
        if (addr >= 0xFF10 && addr <= 0xFF3F) {
            return apu_.audio_register_read(addr);
        }
        return nullptr;
    }
    
    // Call before pausing for FPS management
    void flush_audio() {
        apu_.generate_samples();
    }
};
```

## Audio Channels

The APU implements all four Game Boy sound channels:

1. **Channel 1 (Square with Sweep)**: Square wave with frequency sweep
2. **Channel 2 (Square)**: Simple square wave
3. **Channel 3 (Wave)**: Programmable wave channel with 32-sample wave RAM
4. **Channel 4 (Noise)**: Linear feedback shift register (LFSR) noise generator

## Memory Map

The APU handles the following memory range:

- **Audio Registers**: 0xFF10-0xFF3F
  - 0xFF10-0xFF14: Channel 1 (NR10-NR14)
  - 0xFF16-0xFF19: Channel 2 (NR20-NR24)
  - 0xFF1A-0xFF1E: Channel 3 (NR30-NR34)
  - 0xFF20-0xFF23: Channel 4 (NR40-NR44)
  - 0xFF24: NR50 (Master volume)
  - 0xFF25: NR51 (Sound panning)
  - 0xFF26: NR52 (Sound on/off)
  - 0xFF30-0xFF3F: Wave RAM (16 bytes)

## Timing

- **tick()** must be called **once per M-cycle** (once per CPU instruction)
- **tick_frame_sequencer()** must be called **every 2048 M-cycles** (when bit 10 transitions)
- Audio samples are generated at approximately 48 kHz (21.845 M-cycles per sample)

## Sample Format

Samples are provided as:
- **Format**: Interleaved stereo 16-bit signed integers
- **Layout**: `[L0, R0, L1, R1, L2, R2, ...]`
- **Range**: -32768 to 32767

## Dependencies

The APU library has **zero external dependencies**. It only requires:
- C++20 standard library
- Headers in the `apu/` directory
- `data_structures/stack_vector.h` (included in the library)
