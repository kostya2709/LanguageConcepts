#pragma once

#include <cstddef>
#include <cstdint>
#include <exception>
#include <functional>
#include <variant>

#include "rendezvous.hpp"

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
    // If there was a Consume -> cb_(result)
    // Otherwise -> result_ = result
    result_ = std::move(result);
    if (rendezvous_.Producer()) {
      cb_(std::move(result_));
      delete this;
    }
  }

  void Consume(Callback cb) {
    // If there was a Produce -> cb(result_)
    // Otherwise -> cb_ = cb
    cb_ = std::move(cb);
    if (rendezvous_.Consumer()) {
      cb_(std::move(result_));
      delete this;
    }
  }

 private:
  Callback cb_;
  Result result_;
  Rendezvous rendezvous_;
};

}  // namespace stdlike::detail
