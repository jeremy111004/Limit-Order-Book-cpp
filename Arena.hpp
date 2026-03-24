#include "Order.hpp"
#include <cassert>
#include <cstddef>
#include <memory>

class Arena {
  static constexpr size_t alignment = alignof(Order);

public:
  Arena(size_t N)
      : buffer_(std::unique_ptr<char[]>(new char[N])), ptr_(buffer_.get()),
        Bsize(N) {};
  Arena(const Arena &) = delete;
  Arena &operator=(const Arena &) = delete;

  auto reset() noexcept -> void { ptr_ = buffer_.get(); };
  size_t size() const noexcept { return Bsize; }
  auto used() const noexcept -> size_t {
    assert(ptr_ >= buffer_.get() && "Arena pointer underflow error");
    return static_cast<size_t>(ptr_ - buffer_.get());
  };

  auto allocate(size_t n) -> char * {
    auto size = align_up(n);
    auto remainingSpace =
        static_cast<decltype(size)>(buffer_.get() + Bsize - ptr_);
    if (size <= remainingSpace) {
      char *address = ptr_;
      ptr_ += size;
      return address;
    } else {
      return nullptr;
    }
  };

  auto deallocate(char *p, size_t n) noexcept -> void {};

private:
  static auto align_up(size_t n) noexcept -> size_t {
    return (n + (alignment - 1)) & ~(alignment - 1);
  }

  auto pointer_in_buffer(const char *p) const noexcept -> bool {
    return buffer_.get() <= p && p <= buffer_.get() + Bsize;
  };

  const std::unique_ptr<char[]> buffer_;
  char *ptr_;
  const size_t Bsize;
};
