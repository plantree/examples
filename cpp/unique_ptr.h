#pragma once

#include <memory>

template <typename T, typename Deleter = std::default_delete<T>> class unique_ptr {
public:
  explicit unique_ptr(T *ptr = nullptr)
      : ptr_(ptr) {}

  ~unique_ptr() {
    if (ptr_) {
      Deleter()(ptr_);
    }
  }

  unique_ptr(const unique_ptr &) = delete;
  unique_ptr &operator=(const unique_ptr &) = delete;

  unique_ptr(unique_ptr &&other) noexcept {
    ptr_ = other.ptr_;
    other.ptr_ = nullptr;
  }
  unique_ptr &operator=(unique_ptr &&other) noexcept {
    if (this != &other) {
      if (ptr_) {
        Deleter()(ptr_);
      }
      ptr_ = other.ptr_;
      other.ptr_ = nullptr;
    }
    return *this;
  }

  T *get() const { return ptr_; }

  T *release() {
    T *tmp = ptr_;
    ptr_ = nullptr;
    return tmp;
  }

  void reset(T *ptr = nullptr) {
    if (ptr_) {
      Deleter()(ptr_);
    }
    ptr_ = ptr;
  }

private:
  T *ptr_;
};