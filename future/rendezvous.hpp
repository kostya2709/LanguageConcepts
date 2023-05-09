#pragma once

#include <atomic>

namespace stdlike::detail {

class Rendezvous {
 public:
  Rendezvous() = default;

  // Return true if there was a Consumer() call
  bool Producer() {
    return state_.exchange(true);
  }

  // Return true if there was a Producer() call
  bool Consumer() {
    return state_.exchange(true);
  }

 private:
  std::atomic<bool> state_{false};
};


}  // namespace stdlike::detail
