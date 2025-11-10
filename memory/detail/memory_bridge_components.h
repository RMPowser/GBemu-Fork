#pragma once

#include <array>
#include <cstdint>
#include <utility>


// Callback chain helper
template <typename... Callbacks>
struct CallbackChain {

  template <typename Bus>
  [[gnu::always_inline]] inline static void invoke_write(uint16_t addr, uint8_t value, Bus* bus) {
    (Callbacks{}.write(addr, value, bus), ...);
  }

  template <typename Bus>
  [[gnu::always_inline]] inline static const uint8_t* invoke_read(uint16_t addr, Bus* bus) {
    static_assert(sizeof...(Callbacks) == 1, "Only one callback allowed");
    return (Callbacks{}.read(addr, bus), ...);
  }
};

// Range-based callback registration
template <uint16_t Divisor, uint16_t Start, uint16_t End, typename... Callbacks>
struct RangeCallbacks {
  static constexpr uint16_t start = Start;
  static constexpr uint16_t end = End;

  [[gnu::always_inline]] inline static constexpr bool matches(uint16_t addr) {
    if constexpr (Divisor > 0) {
      static_assert((end - start + 1) % Divisor == 0, "Range must be a multiple of 0x200");
      return addr >= start / Divisor && addr <= end / Divisor;
    } else {
      return addr >= (start - 0xFE00) && addr <= (end - 0xFE00);
    }
  }

  template <typename Bus>
  [[gnu::always_inline]] inline static void invoke_write(uint16_t addr, uint8_t value, Bus* bus) {
    CallbackChain<Callbacks...>::template invoke_write<Bus>(addr, value, bus);
  }

  template <typename Bus>
  [[gnu::always_inline]] inline static const uint8_t* invoke_read(uint16_t addr, Bus* bus) {
    return CallbackChain<Callbacks...>::template invoke_read<Bus>(addr, bus);
  }
};

template <uint16_t Start, uint16_t End, typename... Callbacks>
using FirstLevelRangeCallbacks = RangeCallbacks<0x200, Start, End, Callbacks...>;

template <uint16_t Start, uint16_t End, typename... Callbacks>
using SecondLevelRangeCallbacks = RangeCallbacks<0, Start, End, Callbacks...>;

// Single address callback registration
template <uint16_t Addr, typename... Callbacks>
struct AddressCallbacks {
  static constexpr uint16_t address = Addr;

  [[gnu::always_inline]] inline static constexpr bool matches(uint16_t addr) { return addr == Addr - 0xFE00; }

  template <typename Bus>
  [[gnu::always_inline]] inline static void invoke_write(uint16_t addr, uint8_t value, Bus* bus) {
    CallbackChain<Callbacks...>::template invoke_write<Bus>(addr, value, bus);
  }

  template <typename Bus>
  [[gnu::always_inline]] inline static const uint8_t* invoke_read(uint16_t addr, Bus* bus) {
    return CallbackChain<Callbacks...>::template invoke_read<Bus>(addr, bus);
  }
};

// Collector that gathers all matching callbacks for an address
template <typename Bus, typename... Handlers>
struct WriteCallbackCollector;

template <typename Bus>
struct WriteCallbackCollector<Bus> {
  template <uint16_t Addr>
  [[gnu::always_inline]] inline static void invoke_write(uint16_t addr, uint8_t value, Bus* bus) {}
};

template <typename Bus, typename First, typename... Rest>
struct WriteCallbackCollector<Bus, First, Rest...> {
  template <uint16_t Addr>
  [[gnu::always_inline]] inline static void invoke_write(uint16_t addr, uint8_t value, Bus* bus) {
    if constexpr (First::matches(Addr)) {
      First::template invoke_write<Bus>(addr, value, bus);
    }
    WriteCallbackCollector<Bus, Rest...>::template invoke_write<Addr>(addr, value, bus);
  }
};

template <typename Bus, typename... Handlers>
struct ReadCallbackCollector;

template <typename Bus>
struct ReadCallbackCollector<Bus> {
  template <uint16_t Addr>
  [[gnu::always_inline]] inline static const uint8_t* invoke_read(uint16_t addr, Bus* bus) {
    return nullptr;
  }
};

template <typename Bus, typename First, typename... Rest>
struct ReadCallbackCollector<Bus, First, Rest...> {
  template <uint16_t Addr>
  [[gnu::always_inline]] inline static const uint8_t* invoke_read(uint16_t addr, Bus* bus) {
    if constexpr (First::matches(Addr)) {
      return First::template invoke_read<Bus>(addr, bus);
    }
    return ReadCallbackCollector<Bus, Rest...>::template invoke_read<Addr>(addr, bus);
  }
};

// Generate function for a specific address that checks all handlers
template <typename Bus, uint16_t Addr, typename... Handlers>
struct AddressFunction {
  [[gnu::always_inline]] inline static const uint8_t* read(uint16_t addr, Bus* bus) {
    return ReadCallbackCollector<Handlers...>::template invoke_read<Addr>(addr, bus);
  }

  [[gnu::always_inline]] inline static void write(uint16_t addr, uint8_t value, Bus* bus) {
    WriteCallbackCollector<Handlers...>::template invoke_write<Addr>(addr, value, bus);
  }
};

// Function pointer types
template <typename Bus>
using ReadFunc = const uint8_t* (*)(uint16_t, Bus*);

template <typename Bus>
using WriteFunc = void (*)(uint16_t, uint8_t, Bus*);

// Main memory map with lookup tables
template <typename Bus, typename ReadHandlers, typename WriteHandlers>
struct MemoryBridge;

template <typename Bus, typename... ReadHandlers, typename... WriteHandlers>
struct MemoryBridge<Bus, ReadCallbackCollector<ReadHandlers...>, WriteCallbackCollector<WriteHandlers...>> {

  MemoryBridge(Bus* bus) : bus_(bus) {}
  MemoryBridge() = delete;

  template <size_t... Is>
  static constexpr auto make_read_table(std::index_sequence<Is...>) {
    return std::array<ReadFunc<Bus>, 128>{AddressFunction<Bus, Is, ReadHandlers...>::read...};
  }

  template <size_t... Is>
  static constexpr auto make_write_table(std::index_sequence<Is...>) {
    return std::array<WriteFunc<Bus>, 128>{AddressFunction<Bus, Is, WriteHandlers...>::write...};
  }

  static constexpr auto read_table = make_read_table(std::make_index_sequence<128>{});
  static constexpr auto write_table = make_write_table(std::make_index_sequence<128>{});

  // O(1) lookup
  inline const uint8_t* read(uint16_t addr) { return read_table[addr / 0x200](addr, bus_); }
  inline void write(uint16_t addr, uint8_t value) { write_table[addr / 0x200](addr, value, bus_); }

private:
  Bus* bus_;
};

// Main memory map with lookup tables
template <typename Bus, typename ReadHandlers, typename WriteHandlers>
struct SecondLevelMemoryBridge;

template <typename Bus, typename... ReadHandlers, typename... WriteHandlers>
struct SecondLevelMemoryBridge<Bus, ReadCallbackCollector<ReadHandlers...>,
                               WriteCallbackCollector<WriteHandlers...>> {

  SecondLevelMemoryBridge(Bus* bus) : bus_(bus) {}
  SecondLevelMemoryBridge() = delete;

  template <size_t... Is>
  static constexpr auto make_read_table(std::index_sequence<Is...>) {
    return std::array<ReadFunc<Bus>, 512>{AddressFunction<Bus, Is, ReadHandlers...>::read...};
  }

  template <size_t... Is>
  static constexpr auto make_write_table(std::index_sequence<Is...>) {
    return std::array<WriteFunc<Bus>, 512>{AddressFunction<Bus, Is, WriteHandlers...>::write...};
  }

  static constexpr auto read_table = make_read_table(std::make_index_sequence<512>{});
  static constexpr auto write_table = make_write_table(std::make_index_sequence<512>{});

  // O(1) lookup
  inline const uint8_t* read(uint16_t addr) { return read_table[addr - 0xFE00](addr, bus_); }
  inline void write(uint16_t addr, uint8_t value) { write_table[addr - 0xFE00](addr, value, bus_); }

private:
  Bus* bus_;
};

template <typename Bus, typename... ReadHandlers>
struct ReadHandlerList {
  using type = ReadCallbackCollector<Bus, ReadHandlers...>;
};

template <typename Bus, typename... WriteHandlers>
struct WriteHandlerList {
  using type = WriteCallbackCollector<Bus, WriteHandlers...>;
};