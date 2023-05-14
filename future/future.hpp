#pragma once

#include <exception>
#include <memory>
#include <cassert>

#include <atomic>
#include <optional>

#include "promise.hpp"
#include "future_api.hpp"
#include "shared_state.hpp"

namespace stdlike {

  template <typename T>
  Future<T> Future<T>::Then( std::function<T(T)>&& foo) {
    assert( pool_ );
    Promise<T> next_promise_;
    auto future = next_promise_.MakeFuture(pool_);
    future.prev_future_ = this;
    auto lambda = [foo = std::move(foo), this, next_promise_ = std::move(next_promise_)](T res) mutable {
      next_promise_.SetValue(foo(res));
    };
    hasThen_ = true;
    cb_ = new Future<T>::Derived<decltype(lambda)>(std::move(lambda));
    return future;
  }

}  // namespace stdlike
