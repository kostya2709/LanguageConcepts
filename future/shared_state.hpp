#pragma once

#include <cstddef>
#include <cstdint>
#include <exception>
#include <functional>
#include <variant>

namespace stdlike::detail {

namespace types {
static constexpr size_t kEmpty = 0;
static constexpr size_t kValue = 1;
static constexpr size_t kException = 2;
}  // namespace types

template <typename T>
class SharedState {
 public:
  using Result = std::variant<std::monostate, T, std::exception_ptr>;
  using Callback = std::function<void(Result)>;

  SharedState() = default;
  ~SharedState() = default;

  SharedState(const SharedState&) = delete;
  SharedState& operator=(const SharedState&) = delete;

  SharedState(SharedState&&) = default;
  SharedState& operator=(SharedState&&) = default;

  void Produce(Result result) {
    if (has_result_.exchange(true)) {
      return;
    }
    result_ = std::move(result);

    if (has_callback_.load()) {
      cb_(std::move(result_));
      delete this;
    }
  }

  void Consume(Callback cb) {
    if (has_callback_.exchange(true)) {
      return;
    }
    cb_ = std::move(cb);
    if (has_result_.load()) {
      cb_(std::move(result_));
      delete this;
    }
  }

  bool HasResult() {
    return has_result_.load();
  }

 private:
  Callback cb_;
  Result result_;
  std::atomic<bool> has_callback_{false};
  std::atomic<bool> has_result_{false};
};

}  // namespace stdlike::detail
