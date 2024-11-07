#pragma once

#include <atomic>

template <typename T> class weak_ptr;

struct ControlBlock {
  std::atomic<int> ref_count_;
  std::atomic<int> weak_ref_count_;
  void (*deleter_)(void *);
};

template <typename T> class shared_ptr {
public:
  friend class weak_ptr<T>;
  shared_ptr() noexcept : ptr_(nullptr), control_block_(nullptr) {}

  explicit shared_ptr(T *p) : ptr_(p), control_block_(new ControlBlock()) {
    construct();
  }

  template <typename Deleter>
  shared_ptr(T *p, Deleter d) : ptr_(p), control_block_(new ControlBlock()) {
    construct(d);
  }

  explicit shared_ptr(const weak_ptr<T> &wp) noexcept
      : ptr_(wp.ptr_), control_block_(wp.control_block_) {
    if (control_block_) {
      ++control_block_->ref_count_;
    }
  }

  ~shared_ptr() { clear(); }

  // copy constructor
  shared_ptr(const shared_ptr &other) noexcept
      : ptr_(other.ptr_), control_block_(other.control_block_) {
    if (control_block_) {
      ++control_block_->ref_count_;
    }
  }

  shared_ptr &operator=(const shared_ptr &other) noexcept {
    if (this != &other) {
      clear();
      ptr_ = other.ptr_;
      control_block_ = other.control_block_;
      if (control_block_) {
        ++control_block_->ref_count_;
      }
    }
    return *this;
  }

  // move constructor
  shared_ptr(shared_ptr &&other) noexcept
      : ptr_(other.ptr_), control_block_(other.control_block_) {
    other.ptr_ = nullptr;
    other.control_block_ = nullptr;
  }

  shared_ptr &operator=(shared_ptr &&other) noexcept {
    if (this != &other) {
      clear();
      ptr_ = other.ptr_;
      control_block_ = other.control_block_;
      other.ptr_ = nullptr;
      other.control_block_ = nullptr;
    }
    return *this;
  }

  T *get() const noexcept { return ptr_; }

  int use_count() const noexcept {
    return control_block_ ? control_block_->ref_count_.load() : 0;
  }

  void reset(T *p = nullptr) {
    if (ptr_ != p) {
      clear();
      ptr_ = p;
      if (p) {
        construct();
      } else {
        control_block_ = nullptr;
      }
    }
  }

private:
  T *ptr_;
  ControlBlock *control_block_;

  void clear() {
    if (control_block_ && --control_block_->ref_count_ == 0) {
      control_block_->deleter_(ptr_);
      if (control_block_->weak_ref_count_ == 0) {
        delete control_block_;
      }
    }
  }

  void construct() {
    control_block_->ref_count_ = 1;
    control_block_->weak_ref_count_ = 0;
    control_block_->deleter_ = [](void *ptr) { delete static_cast<T *>(ptr); };
  }

  template <typename Deleter>
  void construct(Deleter d) {
    control_block_->ref_count_ = 1;
    control_block_->weak_ref_count_ = 0;
    control_block_->deleter_ = d;
  }
};

template <typename T> class weak_ptr {
public:
  friend class shared_ptr<T>;
  weak_ptr() noexcept : ptr_(nullptr), control_block_(nullptr) {}

  weak_ptr(const shared_ptr<T> &sp) noexcept
      : ptr_(sp.get()), control_block_(sp.control_block_) {
    if (control_block_) {
      ++control_block_->weak_ref_count_;
    }
  }

  weak_ptr(const weak_ptr &other) noexcept
      : ptr_(other.ptr_), control_block_(other.control_block_) {
    if (control_block_) {
      ++control_block_->weak_ref_count_;
    }
  }

  weak_ptr &operator=(const weak_ptr &other) noexcept {
    if (this != &other) {
      clear();
      ptr_ = other.ptr_;
      control_block_ = other.control_block_;
      if (control_block_) {
        ++control_block_->weak_ref_count_;
      }
    }
    return *this;
  }

  weak_ptr(const weak_ptr &&other) noexcept
      : ptr_(other.ptr_), control_block_(other.control_block_) {
    other.ptr_ = nullptr;
    other.control_block_ = nullptr;
  }

  weak_ptr &operator=(weak_ptr &&other) noexcept {
    if (this != &other) {
      clear();
      ptr_ = other.ptr_;
      control_block_ = other.control_block_;
      other.ptr_ = nullptr;
      other.control_block_ = nullptr;
    }
    return *this;
  }

  ~weak_ptr() { clear(); }

  int use_count() const noexcept {
    return control_block_ ? control_block_->ref_count_.load(std::memory_order_relaxed) : 0;
  }

  bool expired() const noexcept { return use_count() == 0; }

  shared_ptr<T> lock() const noexcept {
    return expired() ? shared_ptr<T>() : shared_ptr<T>(*this);
  }

private:
  T *ptr_;
  ControlBlock *control_block_;

  void clear() {
    if (control_block_ && --control_block_->weak_ref_count_ == 0 &&
        control_block_->ref_count_ == 0) {
      delete control_block_;
    }
  }
};