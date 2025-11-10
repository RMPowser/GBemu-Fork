#include <inttypes.h>
#include <iostream>
#include "main_loop.h"
#include "rom_header.h"
#include "rom_loader.h"

void check_test(CPURegisters& registers) {
  if (registers.B().get() == 3 && registers.C().get() == 5 && registers.D().get() == 8 &&
      registers.E().get() == 13 && registers.H().get() == 21 && registers.L().get() == 34) {
    std::exit(0);
  } else if (registers.B().get() == 0x42 && registers.C().get() == 0x42 && registers.D().get() == 0x42 &&
             registers.E().get() == 0x42 && registers.H().get() == 0x42 && registers.L().get() == 0x42) {
    std::exit(1);
  }
}

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: Rom" << std::endl;
    return -1;
  }

  std::string filename(argv[1]);

  std::string boot_rom_filename;
  if (argc > 2) {
    boot_rom_filename = std::string(argv[2]);
  }
  ROMLoader loader(filename, boot_rom_filename);
  if (!loader.load()) {
    return -1;
  }

  loader.header()->pretty_print();

  OSBridge bridge;
  bridge.blit_screen = [](const uint32_t* pixels, size_t pitch) {
  };
  bridge.present_frame = [](void) {
  };
  bridge.handle_events = [](JoypadState& joypad_state) {
    return false;
  };
  bridge.on_audio_generated = [](const int16_t* samples, int num_samples) {
  };

  MainLoop loop(loader, bridge);

  while (true) {
    loop.run_once();
    check_test(loop.cpu().registers());
  }
  return 0;
}
