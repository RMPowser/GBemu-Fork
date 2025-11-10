#pragma once
#include <string>
#include <vector>
#include "mbc.h"
#include "rom_header.h"

class ROMLoader {
public:
  ROMLoader(const std::string& cartridge_name)
      : cartridge_name_(cartridge_name), should_initialise_mbc_(true) {}
  ROMLoader(const std::string& cartridge_name, const std::string& boot_rom_name)
      : cartridge_name_(cartridge_name), boot_rom_name_(boot_rom_name), should_initialise_mbc_(true) {}
  ROMLoader(const ROMHeader& header);

  bool load();
  std::string get_load_error() const { return load_error_; }
  bool should_initialise_mbc() const;
  const ROMHeader* header() const;
  std::string title() const;
  std::string ram_filename() const;
  uint32_t ram_size() const;
  uint32_t rom_bank_count() const;
  uint32_t ram_bank_count() const;
  ROMType rom_type() const;
  const unsigned char* data(uint32_t address) const;
  const unsigned char* ram_data(uint32_t address) const;
  const unsigned char* boot_rom_data() const { return boot_rom_data_.data(); }
  bool has_boot_rom() const { return !boot_rom_data_.empty(); }

  bool has_battery() const;
  void check_compatibility();

private:
  uint32_t cart_size_ = 0;
  std::vector<unsigned char> data_;
  std::vector<unsigned char> ram_data_;
  std::vector<unsigned char> boot_rom_data_;
  std::string cartridge_name_;
  std::string boot_rom_name_;
  std::string ram_filename_;
  std::string load_error_;
  uint32_t ram_size_ = 0;
  bool should_initialise_mbc_ = true;
};
