#pragma once

#include <exception>
#include <memory>
#include <cassert>

#include <atomic>

#include "shared_state.hpp"

namespace stdlike {

template <typename T>
class Future {
  template <typename U>
  friend class Promise;

  using SharedState = detail::SharedState<T>;
  using SharedStatePtr = SharedState*;
 public:
  // Non-copyable
  Future(const Future&) = delete;
  Future& operator=(const Future&) = delete;

  // Movable
  Future(Future&& other) {
    shared_state_ = other.shared_state_;
    other.shared_state_ = nullptr;
  }
  Future& operator=(Future&& other) {
    shared_state_ = other.shared_state_;
    other.shared_state_ = nullptr;
    return *this;
  }

  ~Future() {
    if (shared_state_) {
      shared_state_->Consume([](typename SharedState::Result){});
    }
  }

  // One-shot
  // Wait for result (value or exception)
  T Get() {
    typename SharedState::Result result;
    std::atomic<uint32_t> state{0};

    shared_state_->Consume([&](typename SharedState::Result cb_result){
      result = std::move(cb_result); 
      state.store(1);
      state.notify_one();
    });
    shared_state_ = nullptr;

    state.wait(0);

    // Unwrap result
    switch (result.index()) {
      case detail::types::kValue:
        return std::move(std::get<detail::types::kValue>(result));
      case detail::types::kException:
        std::rethrow_exception(std::get<detail::types::kException>(result));
        assert(false && "Unreachable");
      default:
        assert(false && "Result is ill-formed");
    }
    assert(false && "Unreachable");
  }

 private:
  explicit Future(SharedStatePtr shared_state) : shared_state_(shared_state) {
  }

 private:
  SharedStatePtr shared_state_;
};

}  // namespace stdlike
