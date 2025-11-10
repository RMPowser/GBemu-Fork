#include "rom_loader.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include "constants.h"
#include "utils.h"

namespace {
constexpr size_t CHUNK_SIZE = 1024;
constexpr uint8_t CART_TYPE_BATTERY_MBC1 = 0x03;
constexpr uint8_t CART_TYPE_BATTERY_MBC2 = 0x09;
constexpr uint8_t CART_TYPE_BATTERY_MBC3 = 0x1B;
constexpr uint8_t CART_TYPE_BATTERY_MBC5 = 0x1E;
constexpr uint8_t CGB_FLAG_CGB_OR_DMG = 0x80;
constexpr uint8_t CGB_FLAG_CGB_ONLY = 0xC0;
constexpr size_t TITLE_LENGTH = 16;
constexpr size_t MIN_ROM_SIZE = ROM_START + sizeof(ROMHeader);
constexpr uint8_t CART_TYPE_ROM_ONLY = 0x00;
constexpr uint8_t CART_TYPE_MBC1 = 0x01;
constexpr uint8_t CART_TYPE_MBC1_RAM = 0x02;
constexpr uint8_t CART_TYPE_MBC1_RAM_BATTERY = 0x03;
constexpr uint8_t CART_TYPE_MBC2 = 0x05;
constexpr uint8_t CART_TYPE_MBC2_BATTERY = 0x06;
constexpr uint8_t CART_TYPE_MBC5 = 0x19;
constexpr uint8_t CART_TYPE_MBC5_RAM = 0x1A;
constexpr uint8_t CART_TYPE_MBC5_RAM_BATTERY = 0x1B;
constexpr uint8_t CART_TYPE_MBC5_RUMBLE = 0x1C;
constexpr uint8_t CART_TYPE_MBC5_RUMBLE_RAM = 0x1D;
constexpr uint8_t CART_TYPE_MBC5_RUMBLE_RAM_BATTERY = 0x1E;
}  // namespace

ROMLoader::ROMLoader(const ROMHeader& header) : should_initialise_mbc_(false) {
  data_.resize(MIN_ROM_SIZE);
  memcpy(data_.data() + ROM_START, &header, sizeof(ROMHeader));
}

bool ROMLoader::load() {
  std::ifstream file(cartridge_name_, std::ios::binary);
  if (!file) {
    std::cerr << "Failed to open file: " << cartridge_name_ << std::endl;
    return false;
  }

  // Get file size for efficient reading
  file.seekg(0, std::ios::end);
  std::streamsize file_size = file.tellg();
  file.seekg(0, std::ios::beg);

  if (file_size < static_cast<std::streamsize>(MIN_ROM_SIZE)) {
    std::cerr << "ROM file too small: " << file_size << " bytes (minimum " << MIN_ROM_SIZE << " bytes)"
              << std::endl;
    return false;
  }

  // Reserve space for efficiency
  data_.reserve(static_cast<size_t>(file_size));
  char buffer[CHUNK_SIZE];

  while (file.read(buffer, CHUNK_SIZE) || file.gcount() > 0) {
    size_t available_bytes = static_cast<size_t>(file.gcount());
    data_.insert(data_.end(), buffer, buffer + available_bytes);
  }

  cart_size_ = data_.size();
  std::cout << "Read " << data_.size() << " bytes from " << cartridge_name_ << std::endl;

  if (has_battery()) {
    // Get filename with full path, but replace extension with ".ram"
    ram_filename_ = cartridge_name_;
    size_t last_dot = ram_filename_.find_last_of('.');
    if (last_dot != std::string::npos) {
      ram_filename_ = ram_filename_.substr(0, last_dot);
    }
    ram_filename_ += ".ram";

    std::ifstream ramfile(ram_filename_, std::ios::binary);
    if (ramfile) {
      ramfile.seekg(0, std::ios::end);
      std::streamsize ramsize = ramfile.tellg();
      ramfile.seekg(0, std::ios::beg);
      if (ramsize > 0) {
        uint32_t expected_ram_size = ram_bank_count() * 8192;  // 8KB per RAM bank
        if (expected_ram_size > 0 && static_cast<uint32_t>(ramsize) != expected_ram_size) {
          std::cerr << "Warning: RAM file size (" << ramsize << " bytes) doesn't match expected size ("
                    << expected_ram_size << " bytes)" << std::endl;
        }
        size_t ram_size_to_load = static_cast<size_t>(std::min(
            ramsize, static_cast<std::streamsize>(expected_ram_size > 0 ? expected_ram_size : ramsize)));
        ram_data_.resize(ram_size_to_load);
        ramfile.read(reinterpret_cast<char*>(ram_data_.data()),
                     static_cast<std::streamsize>(ram_size_to_load));
        std::cout << "Loaded RAM data from " << ram_filename_ << " (" << ram_size_to_load << " bytes)"
                  << std::endl;
        ram_size_ = static_cast<uint32_t>(ram_size_to_load);
      }
    }
  }

  std::cout << "Cart size: " << cart_size_ << std::endl;

  // Load boot ROM if specified
  if (!boot_rom_name_.empty()) {
    std::ifstream boot_file(boot_rom_name_, std::ios::binary);
    if (!boot_file) {
      load_error_ = "Failed to open boot ROM file:\n" + boot_rom_name_;
      std::cerr << load_error_ << std::endl;
      return false;
    }

    // Get boot ROM size
    boot_file.seekg(0, std::ios::end);
    std::streamsize boot_size = boot_file.tellg();
    boot_file.seekg(0, std::ios::beg);

    // Verify boot ROM is exactly 256 bytes
    if (boot_size != 256) {
      load_error_ = "Boot ROM must be exactly 256 bytes.\nGot " + std::to_string(boot_size) + " bytes.";
      std::cerr << load_error_ << std::endl;
      return false;
    }

    // Load boot ROM data
    boot_rom_data_.resize(256);
    boot_file.read(reinterpret_cast<char*>(boot_rom_data_.data()), 256);

    if (!boot_file) {
      load_error_ = "Failed to read boot ROM data from:\n" + boot_rom_name_;
      std::cerr << load_error_ << std::endl;
      boot_rom_data_.clear();
      return false;
    }

    std::cout << "Loaded boot ROM from " << boot_rom_name_ << " (256 bytes)" << std::endl;
  }

  return true;
}

const ROMHeader* ROMLoader::header() const {
  if (data_.size() < MIN_ROM_SIZE) {
    return nullptr;
  }
  return reinterpret_cast<const ROMHeader*>(data_.data() + ROM_START);
}

const unsigned char* ROMLoader::ram_data(uint32_t address) const {
  if (address >= ram_data_.size()) {
    return nullptr;
  }
  return ram_data_.data() + address;
}

bool ROMLoader::has_battery() const {
  const ROMHeader* hdr = header();
  if (!hdr) {
    return false;
  }
  switch (hdr->cart_type) {
    case CART_TYPE_BATTERY_MBC1:
    case CART_TYPE_BATTERY_MBC2:
    case CART_TYPE_BATTERY_MBC3:
    case CART_TYPE_BATTERY_MBC5:
      return true;
    default:
      return false;
  }
}

uint32_t ROMLoader::rom_bank_count() const {
  const ROMHeader* hdr = header();
  if (!hdr) {
    return 0;
  }
  switch (hdr->rom_size) {
    case 0:
      return 2;
    case 1:
      return 4;
    case 2:
      return 8;
    case 3:
      return 16;
    case 4:
      return 32;
    case 5:
      return 64;
    case 6:
      return 128;
    case 7:
      return 256;
    case 8:
      return 512;
    default:
      FATAL("Invalid ROM size");
  }
}

uint32_t ROMLoader::ram_bank_count() const {
  const ROMHeader* hdr = header();
  if (!hdr) {
    return 0;
  }
  switch (hdr->ram_size) {
    case 0:
      return 0;
    case 1:
      return 1;
    case 2:
      return 1;
    case 3:
      return 4;
    case 4:
      return 16;
    case 5:
      return 8;
    default:
      FATAL("Invalid RAM size");
  }
}

const unsigned char* ROMLoader::data(uint32_t address) const {
  if (address >= data_.size()) {
    return nullptr;
  }
  return data_.data() + address;
}

std::string ROMLoader::title() const {
  const ROMHeader* hdr = header();
  if (!hdr) {
    return "";
  }
  return std::string(hdr->title, TITLE_LENGTH);
}

void ROMLoader::check_compatibility() {
  const ROMHeader* hdr = header();
  if (!hdr) {
    FATAL("Cannot check compatibility: invalid ROM header");
    return;
  }
  CGBFlag cgb_flag = CGBFlag::DMG;
  const uint8_t cgb_flag_value = static_cast<uint8_t>(hdr->title[TITLE_LENGTH - 1]);
  if (cgb_flag_value == CGB_FLAG_CGB_OR_DMG) {
    cgb_flag = CGBFlag::CGB_OR_DMG;
  } else if (cgb_flag_value == CGB_FLAG_CGB_ONLY) {
    cgb_flag = CGBFlag::CGB_ONLY;
  }

  if (cgb_flag == CGBFlag::CGB_ONLY) {
    FATAL("CGB only ROMs are not supported");
  }
}

ROMType ROMLoader::rom_type() const {
  const ROMHeader* hdr = header();
  if (!hdr) {
    FATAL("Cannot determine ROM type: invalid ROM header");
    return ROMType::MBC1;  // Default fallback
  }
  switch (hdr->cart_type) {
    case CART_TYPE_ROM_ONLY:
    case CART_TYPE_MBC1:
    case CART_TYPE_MBC1_RAM:
    case CART_TYPE_MBC1_RAM_BATTERY:
      return ROMType::MBC1;
    case CART_TYPE_MBC2:
    case CART_TYPE_MBC2_BATTERY:
      return ROMType::MBC2;
    case CART_TYPE_MBC5:
    case CART_TYPE_MBC5_RAM:
    case CART_TYPE_MBC5_RAM_BATTERY:
    case CART_TYPE_MBC5_RUMBLE:
    case CART_TYPE_MBC5_RUMBLE_RAM:
    case CART_TYPE_MBC5_RUMBLE_RAM_BATTERY:
      return ROMType::MBC5;
    default:
      FATAL("Unsupported ROM type: " + std::to_string(hdr->cart_type));
  }
}

std::string ROMLoader::ram_filename() const {
  return ram_filename_;
}

uint32_t ROMLoader::ram_size() const {
  return ram_size_;
}

bool ROMLoader::should_initialise_mbc() const {
  return should_initialise_mbc_;
}