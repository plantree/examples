#pragma once

using default_deleter = void (*)(void *);

template <typename T, typename Deleter = default_deleter> class unique_ptr {
public:
  explicit unique_ptr(T *ptr = nullptr)
      : ptr_(ptr), deleter_([](void *p) { delete static_cast<T *>(p); }) {}

  ~unique_ptr() {
    if (ptr_) {
      deleter_(ptr_);
    }
  }

  unique_ptr(const unique_ptr &) = delete;
  unique_ptr &operator=(const unique_ptr &) = delete;

  unique_ptr(unique_ptr &&other) noexcept {
    ptr_ = other.ptr_;
    deleter_ = other.deleter_;
    other.ptr_ = nullptr;
    other.deleter_ = nullptr;
  }
  unique_ptr &operator=(unique_ptr &&other) noexcept {
    if (this != &other) {
      if (ptr_) {
        deleter_(ptr_);
      }
      ptr_ = other.ptr_;
      deleter_ = other.deleter_;
      other.ptr_ = nullptr;
      other.deleter_ = nullptr;
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
      deleter_(ptr_);
    }
    ptr_ = ptr;
  }

private:
  T *ptr_;
  Deleter deleter_;
};