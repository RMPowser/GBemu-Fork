#pragma once

#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#include "stack_vector.h"

class SaveStateSerializer;

namespace {
constexpr uint32_t SERIALIZER_VERSION = 1;

template <typename T>
concept IsNotPointer = !std::is_pointer_v<T>;

template <typename T>
concept Integral = std::is_integral_v<T>;

template <typename T>
concept SimpleType = std::is_standard_layout_v<T> || std::is_arithmetic_v<T> || std::is_enum_v<T>;

template <typename T>
concept KnownSpecialType =
    std::same_as<std::remove_cv_t<T>, std::string> || std::same_as<std::remove_cv_t<T>, std::string_view>;

template <typename T>
concept ComplexObject = !SimpleType<T> && !KnownSpecialType<T>;

template <typename T>
concept HasSerializeMethod = requires(T t, SaveStateSerializer& s) {
  { t.serialize(s) } -> std::same_as<void>;
};

template <typename T>
concept HasPushBack = requires(T t) {
  { t.push_back(std::declval<typename T::value_type>()) } -> std::same_as<void>;
};

template <typename T>
struct is_vector_like : std::false_type {};

template <typename U>
struct is_vector_like<std::vector<U>> : std::true_type {};

template <typename U, size_t N>
struct is_vector_like<StackVector<U, N>> : std::true_type {};

template <typename T>
concept IsVectorType = is_vector_like<T>::value && HasPushBack<T>;

}  // namespace

class SaveStateSerializer {
public:
  SaveStateSerializer(const std::string& filepath, bool for_reading) : for_reading_(for_reading) {
    std::ios::openmode mode = std::ios::binary;
    if (for_reading_) {
      mode |= std::ios::in;
    } else {
      mode |= std::ios::out;
    }

    stream_.open(filepath, mode);
    if (!stream_.is_open()) {
      throw std::runtime_error("Failed to open save state file: " + filepath);
    }
  }

  ~SaveStateSerializer() {
    if (stream_.is_open()) {
      stream_.close();
    }
  }

  // Check if the serializer is in a valid state
  bool is_valid() const { return stream_.is_open() && stream_.good(); }

  template <typename T>
  void print_index() {
    if (false)
      std::cout << "Index: " << stream_.tellp() << " for type: " << typeid(T).name() << std::endl;
  }

  template <typename T>
    requires IsNotPointer<T> && (Integral<T> || std::is_floating_point_v<T>)
  SaveStateSerializer& operator<<(const T& value) {
    print_index<T>();
    stream_.write(reinterpret_cast<const char*>(&value), sizeof(T));
    return *this;
  }

  SaveStateSerializer& operator<<(const std::string& value) {
    print_index<std::string>();
    uint32_t length = static_cast<uint32_t>(value.length());
    stream_.write(reinterpret_cast<const char*>(&length), sizeof(length));
    stream_.write(value.data(), length);
    return *this;
  }

  template <typename T>
    requires ComplexObject<T> && HasSerializeMethod<T>
  SaveStateSerializer& operator<<(T& value) {
    print_index<T>();
    value.serialize(*this);
    return *this;
  }

  template <typename T>
    requires IsNotPointer<T> && std::is_standard_layout_v<T> && (!IsVectorType<T>) && (!Integral<T>) &&
             (!std::is_floating_point_v<T>)
  SaveStateSerializer& operator<<(const T& value) {
    print_index<T>();
    stream_.write(reinterpret_cast<const char*>(&value), sizeof(T));
    return *this;
  }

  template <typename T>
    requires IsVectorType<T>
  SaveStateSerializer& operator<<(const T& value) {
    uint32_t size = static_cast<uint32_t>(value.size());
    print_index<T>();
    stream_.write(reinterpret_cast<const char*>(&size), sizeof(size));
    for (const auto& item : value) {
      *this << item;
    }
    return *this;
  }

  template <typename T>
    requires IsNotPointer<T> && (Integral<T> || std::is_floating_point_v<T>)
  SaveStateSerializer& operator>>(T& value) {
    print_index<T>();
    stream_.read(reinterpret_cast<char*>(&value), sizeof(T));
    return *this;
  }

  SaveStateSerializer& operator>>(std::string& value) {
    print_index<std::string>();
    uint32_t length;
    stream_.read(reinterpret_cast<char*>(&length), sizeof(length));
    value.resize(length);
    stream_.read(value.data(), length);
    return *this;
  }

  template <typename T>
    requires ComplexObject<T> && HasSerializeMethod<T>
  SaveStateSerializer& operator>>(T& value) {
    print_index<T>();
    value.deserialize(*this);
    return *this;
  }

  template <typename T>
    requires IsNotPointer<T> && std::is_standard_layout_v<T> && (!IsVectorType<T>) && (!Integral<T>) &&
             (!std::is_floating_point_v<T>)
  SaveStateSerializer& operator>>(T& value) {
    print_index<T>();
    stream_.read(reinterpret_cast<char*>(&value), sizeof(T));
    return *this;
  }

  template <typename T>
  SaveStateSerializer& operator>>(std::vector<T>& value) {
    print_index<std::vector<T>>();
    value.clear();
    uint32_t size;
    stream_.read(reinterpret_cast<char*>(&size), sizeof(size));
    value.reserve(size);
    for (uint32_t i = 0; i < size; i++) {
      T item;
      *this >> item;
      value.push_back(item);
    }
    return *this;
  }

  template <typename T, size_t N>
  SaveStateSerializer& operator>>(StackVector<T, N>& value) {
    print_index<StackVector<T, N>>();
    value.clear();
    uint32_t size;
    stream_.read(reinterpret_cast<char*>(&size), sizeof(size));
    for (uint32_t i = 0; i < size; i++) {
      T item;
      *this >> item;
      value.push_back(item);
    }
    return *this;
  }

private:
  std::fstream stream_;
  bool for_reading_;
};
