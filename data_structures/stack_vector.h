#pragma once

#include <array>
#include <cstddef>
#include <iostream>
#include <iterator>


#define FATAL(x)                 \
  do {                           \
    std::cout << x << std::endl; \
    std::exit(-1);               \
  } while (0)

template <typename T, size_t N>
class StackVector {
public:
  using value_type = T;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using reference = T&;
  using const_reference = const T&;
  using pointer = T*;
  using const_pointer = const T*;
  using iterator = T*;
  using const_iterator = const T*;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  // Constructors
  StackVector() : current_size_(0) {}

  StackVector(size_type count, const T& value) : current_size_(0) {
    if (count > N) {
      FATAL("StackVector: count exceeds capacity");
    }
    for (size_type i = 0; i < count; ++i) {
      data_[i] = value;
    }
    current_size_ = count;
  }

  explicit StackVector(size_type count) : current_size_(0) {
    if (count > N) {
      FATAL("StackVector: count exceeds capacity");
    }
    for (size_type i = 0; i < count; ++i) {
      data_[i] = T();
    }
    current_size_ = count;
  }

  template <typename InputIt>
  StackVector(InputIt first, InputIt last) : current_size_(0) {
    size_type count = 0;
    for (InputIt it = first; it != last; ++it) {
      if (count >= N) {
        FATAL("StackVector: range exceeds capacity");
      }
      data_[count++] = *it;
    }
    current_size_ = count;
  }

  StackVector(std::initializer_list<T> init) : current_size_(0) {
    if (init.size() > N) {
      FATAL("StackVector: initializer_list exceeds capacity");
    }
    size_type i = 0;
    for (const auto& item : init) {
      data_[i++] = item;
    }
    current_size_ = init.size();
  }

  // Copy constructor
  StackVector(const StackVector& other) : current_size_(other.current_size_) {
    for (size_type i = 0; i < current_size_; ++i) {
      data_[i] = other.data_[i];
    }
  }

  // Move constructor
  StackVector(StackVector&& other) noexcept : current_size_(other.current_size_) {
    for (size_type i = 0; i < current_size_; ++i) {
      data_[i] = std::move(other.data_[i]);
    }
    other.current_size_ = 0;
  }

  // Assignment operators
  StackVector& operator=(const StackVector& other) {
    if (this != &other) {
      current_size_ = other.current_size_;
      for (size_type i = 0; i < current_size_; ++i) {
        data_[i] = other.data_[i];
      }
    }
    return *this;
  }

  StackVector& operator=(StackVector&& other) noexcept {
    if (this != &other) {
      current_size_ = other.current_size_;
      for (size_type i = 0; i < current_size_; ++i) {
        data_[i] = std::move(other.data_[i]);
      }
      other.current_size_ = 0;
    }
    return *this;
  }

  StackVector& operator=(std::initializer_list<T> init) {
    if (init.size() > N) {
      FATAL("StackVector: initializer_list exceeds capacity");
    }
    current_size_ = init.size();
    size_type i = 0;
    for (const auto& item : init) {
      data_[i++] = item;
    }
    return *this;
  }

  // Element access
  reference at(size_type pos) {
    if (pos >= current_size_) {
      FATAL("StackVector: at() out of range");
    }
    return data_[pos];
  }

  const_reference at(size_type pos) const {
    if (pos >= current_size_) {
      FATAL("StackVector: at() out of range");
    }
    return data_[pos];
  }

  reference operator[](size_type pos) { return data_[pos]; }

  const_reference operator[](size_type pos) const { return data_[pos]; }

  reference front() {
    if (current_size_ == 0) {
      FATAL("StackVector: front() called on empty vector");
    }
    return data_[0];
  }

  const_reference front() const {
    if (current_size_ == 0) {
      FATAL("StackVector: front() called on empty vector");
    }
    return data_[0];
  }

  reference back() {
    if (current_size_ == 0) {
      FATAL("StackVector: back() called on empty vector");
    }
    return data_[current_size_ - 1];
  }

  const_reference back() const {
    if (current_size_ == 0) {
      FATAL("StackVector: back() called on empty vector");
    }
    return data_[current_size_ - 1];
  }

  T* data() noexcept { return data_.data(); }

  const T* data() const noexcept { return data_.data(); }

  // Iterators
  iterator begin() noexcept { return data_.data(); }

  const_iterator begin() const noexcept { return data_.data(); }

  const_iterator cbegin() const noexcept { return data_.data(); }

  iterator end() noexcept { return data_.data() + current_size_; }

  const_iterator end() const noexcept { return data_.data() + current_size_; }

  const_iterator cend() const noexcept { return data_.data() + current_size_; }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

  const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

  const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }

  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

  const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

  const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

  // Capacity
  bool empty() const noexcept { return current_size_ == 0; }

  size_type size() const noexcept { return current_size_; }

  size_type max_size() const noexcept { return N; }

  size_type capacity() const noexcept { return N; }

  void reserve(size_type new_cap) {
    if (new_cap > N) {
      FATAL("StackVector: reserve() exceeds capacity");
    }
  }

  // Modifiers
  void clear() noexcept { current_size_ = 0; }

  iterator insert(const_iterator pos, const T& value) {
    if (current_size_ >= N) {
      FATAL("StackVector: insert() would exceed capacity");
    }
    size_type index = pos - begin();
    for (size_type i = current_size_; i > index; --i) {
      data_[i] = std::move(data_[i - 1]);
    }
    data_[index] = value;
    ++current_size_;
    return begin() + index;
  }

  iterator insert(const_iterator pos, T&& value) {
    if (current_size_ >= N) {
      FATAL("StackVector: insert() would exceed capacity");
    }
    size_type index = pos - begin();
    for (size_type i = current_size_; i > index; --i) {
      data_[i] = std::move(data_[i - 1]);
    }
    data_[index] = std::move(value);
    ++current_size_;
    return begin() + index;
  }

  iterator insert(const_iterator pos, size_type count, const T& value) {
    if (current_size_ + count > N) {
      FATAL("StackVector: insert() would exceed capacity");
    }
    size_type index = pos - begin();
    for (size_type i = current_size_ + count; i > index + count; --i) {
      data_[i - 1] = std::move(data_[i - 1 - count]);
    }
    for (size_type i = 0; i < count; ++i) {
      data_[index + i] = value;
    }
    current_size_ += count;
    return begin() + index;
  }

  template <typename InputIt>
  iterator insert(const_iterator pos, InputIt first, InputIt last) {
    size_type count = 0;
    for (InputIt it = first; it != last; ++it) {
      ++count;
    }
    if (current_size_ + count > N) {
      FATAL("StackVector: insert() would exceed capacity");
    }
    size_type index = pos - begin();
    for (size_type i = current_size_ + count; i > index + count; --i) {
      data_[i - 1] = std::move(data_[i - 1 - count]);
    }
    size_type i = 0;
    for (InputIt it = first; it != last; ++it, ++i) {
      data_[index + i] = *it;
    }
    current_size_ += count;
    return begin() + index;
  }

  iterator insert(const_iterator pos, std::initializer_list<T> init) {
    return insert(pos, init.begin(), init.end());
  }

  template <typename... Args>
  iterator emplace(const_iterator pos, Args&&... args) {
    if (current_size_ >= N) {
      FATAL("StackVector: emplace() would exceed capacity");
    }
    size_type index = pos - begin();
    for (size_type i = current_size_; i > index; --i) {
      data_[i] = std::move(data_[i - 1]);
    }
    data_[index] = T(std::forward<Args>(args)...);
    ++current_size_;
    return begin() + index;
  }

  iterator erase(const_iterator pos) {
    if (pos == end()) {
      return end();
    }
    size_type index = pos - begin();
    for (size_type i = index; i < current_size_ - 1; ++i) {
      data_[i] = std::move(data_[i + 1]);
    }
    --current_size_;
    return begin() + index;
  }

  iterator erase(const_iterator first, const_iterator last) {
    if (first == last) {
      return const_cast<iterator>(first);
    }
    size_type first_index = first - begin();
    size_type last_index = last - begin();
    size_type count = last_index - first_index;
    for (size_type i = first_index; i < current_size_ - count; ++i) {
      data_[i] = std::move(data_[i + count]);
    }
    current_size_ -= count;
    return begin() + first_index;
  }

  void push_back(const T& value) {
    if (current_size_ >= N) {
      FATAL("StackVector: push_back() would exceed capacity");
    }
    data_[current_size_++] = value;
  }

  void push_back(T&& value) {
    if (current_size_ >= N) {
      FATAL("StackVector: push_back() would exceed capacity");
    }
    data_[current_size_++] = std::move(value);
  }

  template <typename... Args>
  void emplace_back(Args&&... args) {
    if (current_size_ >= N) {
      FATAL("StackVector: emplace_back() would exceed capacity");
    }
    data_[current_size_++] = T(std::forward<Args>(args)...);
  }

  void pop_back() {
    if (current_size_ == 0) {
      FATAL("StackVector: pop_back() called on empty vector");
    }
    --current_size_;
  }

  void pop_front() {
    if (current_size_ == 0) {
      FATAL("StackVector: pop_front() called on empty vector");
    }
    for (size_type i = 0; i < current_size_ - 1; ++i) {
      data_[i] = std::move(data_[i + 1]);
    }
    --current_size_;
  }

  void resize(size_type count) {
    if (count > N) {
      FATAL("StackVector: resize() exceeds capacity");
    }
    if (count > current_size_) {
      for (size_type i = current_size_; i < count; ++i) {
        data_[i] = T();
      }
    }
    current_size_ = count;
  }

  void resize(size_type count, const T& value) {
    if (count > N) {
      FATAL("StackVector: resize() exceeds capacity");
    }
    if (count > current_size_) {
      for (size_type i = current_size_; i < count; ++i) {
        data_[i] = value;
      }
    }
    current_size_ = count;
  }

  void swap(StackVector& other) noexcept {
    std::swap(data_, other.data_);
    std::swap(current_size_, other.current_size_);
  }

private:
  std::array<T, N> data_;
  size_type current_size_;
};

// Comparison operators
template <typename T, size_t N>
bool operator==(const StackVector<T, N>& lhs, const StackVector<T, N>& rhs) {
  if (lhs.size() != rhs.size()) {
    return false;
  }
  for (size_t i = 0; i < lhs.size(); ++i) {
    if (lhs[i] != rhs[i]) {
      return false;
    }
  }
  return true;
}

template <typename T, size_t N>
bool operator!=(const StackVector<T, N>& lhs, const StackVector<T, N>& rhs) {
  return !(lhs == rhs);
}

template <typename T, size_t N>
bool operator<(const StackVector<T, N>& lhs, const StackVector<T, N>& rhs) {
  size_t min_size = lhs.size() < rhs.size() ? lhs.size() : rhs.size();
  for (size_t i = 0; i < min_size; ++i) {
    if (lhs[i] < rhs[i]) {
      return true;
    }
    if (lhs[i] > rhs[i]) {
      return false;
    }
  }
  return lhs.size() < rhs.size();
}

template <typename T, size_t N>
bool operator<=(const StackVector<T, N>& lhs, const StackVector<T, N>& rhs) {
  return !(rhs < lhs);
}

template <typename T, size_t N>
bool operator>(const StackVector<T, N>& lhs, const StackVector<T, N>& rhs) {
  return rhs < lhs;
}

template <typename T, size_t N>
bool operator>=(const StackVector<T, N>& lhs, const StackVector<T, N>& rhs) {
  return !(lhs < rhs);
}
