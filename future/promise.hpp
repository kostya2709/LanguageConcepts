#pragma once

#include "future.hpp"

#include <memory>
#include "shared_state.hpp"

namespace stdlike {

template <typename T>
class Promise {
  using SharedState = detail::SharedState<T>;
  using SharedStatePtr = SharedState*;
 public:
  Promise() : shared_state_(new SharedState()) {
  }

  // Non-copyable
  Promise(const Promise&) = delete;
  Promise& operator=(const Promise&) = delete;

  // Movable
  Promise(Promise&&) = default;
  Promise& operator=(Promise&&) = default;

  // One-shot
  Future<T> MakeFuture() {
    return Future<T>(shared_state_);
  }

  // One-shot
  // Fulfill promise with value
  void SetValue(T value) {
    typename SharedState::Result result;
    result.template emplace<detail::types::kValue>(std::move(value));
    shared_state_->Produce(std::move(result));
  }

  // One-shot
  // Fulfill promise with exception
  void SetException(std::exception_ptr ptr) {
    typename SharedState::Result result;
    result.template emplace<detail::types::kException>(ptr);
    shared_state_->Produce(std::move(result));
  }

 private:
  SharedStatePtr shared_state_;
};

}  // namespace stdlike
