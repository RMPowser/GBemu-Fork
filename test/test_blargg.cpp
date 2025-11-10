#include <inttypes.h>
#include <iostream>
#include "main_loop.h"
#include "rom_header.h"
#include "rom_loader.h"

std::string previous_test_output;

void check_test(std::string& test_output) {
  if (!test_output.empty() && test_output != previous_test_output) {
    std::cout << "Test output: " << test_output << std::endl;
    previous_test_output = test_output;
  }
  if (test_output.find("Passed") != std::string::npos) {
    std::exit(0);
  } else if (test_output.find("Failed") != std::string::npos) {
    std::exit(1);
  }
}

void write_callback(MainLoop& loop, uint16_t address, uint8_t value, std::string& test_output) {
  //This is for the halt test bug rom
  if (address == 0xA000 && value == 0) {
    test_output += "Passed";
  }

  if (address == 0xA000 && value == 1) {
    test_output += "Failed";
  }

  if (address == 0xFF02) {
    unsigned char sb = loop.cpu().hardware_registers().get_SB();
    unsigned char sc = loop.cpu().hardware_registers().get_SC();
    if (sb >= 32 && sb <= 126) {
      std::cout << "Data: " << sb << " SC: " << (int)sc << std::endl;
      test_output += sb;
    }
  }
}

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: Rom" << std::endl;
    return -1;
  }
  std::string boot_rom_filename;
  if (argc > 2) {
    boot_rom_filename = std::string(argv[2]);
  }

  std::string filename(argv[1]);
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
  std::string test_output;
  loop.cpu().mc().set_write_callback(std::bind(write_callback, std::ref(loop), std::placeholders::_1,
                                               std::placeholders::_2, std::ref(test_output)));

  while (true) {
    loop.run_once();
    check_test(test_output);
  }
  return 0;
}
