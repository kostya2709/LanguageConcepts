#pragma once

#include <exception>
#include <memory>
#include <cassert>

#include <atomic>
#include <optional>

#include "promise.hpp"
#include "shared_state.hpp"
#include "thread_pool.hpp"

namespace stdlike {

template <typename T>
class Future {
  template <typename U>
  friend class Promise;

  struct Base {
    virtual ~Base() {};
    virtual void operator()(T param) = 0;
  };

  template <typename U>
  struct Derived : Base {
    U obj;
    
    void operator()(T param) override {
        obj(param);
    }
    Derived( U&& obj) : obj(std::move(obj)) {}
    ~Derived() override {}
  };

  using SharedState = detail::SharedState<T>;
  using SharedStatePtr = SharedState*;
  using NextFoo = std::function<T(T)>;
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
    // delete cb_;
  }

  // Wait for result (value or exception)
  T Get() {

    if ( prev_future_ ) {
        prev_future_->Get();
    }

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
        {
            auto res = std::get<detail::types::kValue>(result);
            if (hasThen_) {
                pool_->Execute([this, res](){(*cb_)(res);});
            }
            return std::move(res);
        }
      case detail::types::kException:
        std::rethrow_exception(std::get<detail::types::kException>(result));
        assert(false && "Unreachable");
      default:
        assert(false && "Result is ill-formed");
    }
    assert(false && "Unreachable");
  }

  std::optional<T> TryGet() {
    if (shared_state_->HasResult()) {
      return Get();
    } else {
      return std::nullopt;
    }
  }

  Future<T> Then( std::function<T(T)>&& foo);

  

 private:
  explicit Future(SharedStatePtr shared_state) : shared_state_(shared_state) {
  }

    Future(SharedStatePtr shared_state, ThreadPool* pool) : shared_state_(shared_state), pool_(pool) {
  }

 private:
  SharedStatePtr shared_state_;
  ThreadPool* pool_{nullptr};
  bool hasThen_{false};
  Base* cb_{nullptr};
  Future<T>* prev_future_{nullptr};
};

}  // namespace stdlike