#pragma once

#include <inttypes.h>
#include <iomanip>
#include <iostream>

struct ROMHeader {
  char entry_code[4];
  char logo[48];
  char title[16];
  char licence_code[2];
  char sgb_flag;
  char cart_type;
  char rom_size;
  char ram_size;
  char destination_code;
  char old_licencee_code;
  char mask_rom_version;
  uint8_t header_checksum;
  uint16_t global_checksum;

  void pretty_print() const {
    std::cout << "ROM Header Information:" << std::endl;

    std::cout << "Entry Code: ";
    for (int i = 0; i < 4; i++) {
      std::cout << std::hex << std::setw(2) << std::setfill('0')
                << (static_cast<unsigned int>(static_cast<unsigned char>(entry_code[i]))) << " ";
    }
    std::cout << std::dec << std::endl;

    std::cout << "Title: ";
    for (int i = 0; i < 16; ++i) {
      if (title[i] == 0)
        break;
      std::cout << title[i];
    }
    std::cout << std::endl;

    std::cout << "Licence Code: ";
    for (int i = 0; i < 2; ++i) {
      std::cout << (int)licence_code[i];
    }
    std::cout << std::endl;

    std::cout << "SGB Flag: " << static_cast<int>(sgb_flag) << std::endl;
    std::cout << "Cart Type: " << static_cast<int>(cart_type) << std::endl;
    std::cout << "ROM Size: " << static_cast<int>(rom_size) << std::endl;
    std::cout << "RAM Size: " << static_cast<int>(ram_size) << std::endl;
    std::cout << "Destination Code: " << static_cast<int>(destination_code) << std::endl;
    std::cout << "Old Licencee Code: " << static_cast<int>(old_licencee_code) << std::endl;
    std::cout << "Mask ROM Version: " << static_cast<int>(mask_rom_version) << std::endl;
    std::cout << "Header Checksum: " << static_cast<int>(header_checksum) << std::endl;
    std::cout << "Global Checksum: " << global_checksum << std::endl;
    std::cout << std::endl;
  }
};
