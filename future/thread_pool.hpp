#pragma once

#include <thread>
#include <atomic>
#include <map>
#include <set>
#include <mutex>

class ThreadPool {

    void killDead() {
        const std::lock_guard<std::mutex> lock1(mutex_dead_);
        for ( const auto& dead_idx : dead_ ) {
            const std::lock_guard<std::mutex> lock2(mutex_threads_);
            auto elem_iter = threads_.find(dead_idx);
            assert(elem_iter != threads_.end());
            elem_iter->second.join();
            threads_.erase( elem_iter);
        }
        dead_.clear();
    }

public:
    ThreadPool( const int thread_n) : thread_n(thread_n) {}

    template <typename Functor, typename ...Args>
    void Execute( Functor&& foo, Args&&... args) {
        killDead();
        if (occupied_.fetch_add(1) >= thread_n) {
            occupied_.fetch_sub(1);
            foo(std::forward<Args>(args)...);
        } else {
            auto ticket = thread_ticket_.fetch_add(1);
            const std::lock_guard<std::mutex> lock1(mutex_threads_);
            threads_.insert( std::make_pair( ticket, 
                                std::thread( [... args = std::forward<Args>(args...), this, ticket, foo=std::move(foo)]() mutable {
                                    foo(std::forward<Args>(args)...);

                                    const std::lock_guard<std::mutex> lock1(mutex_dead_);
                                    dead_.insert(ticket);

                                    occupied_.fetch_sub(1);

                                    occupied_.notify_one();
                                })));
        }
    }
    ~ThreadPool() {
        int old = 0;
        while ( (old = occupied_.load()) ) {
            occupied_.wait(old);
        }
        killDead();
    }
private:
    const int thread_n;
    std::atomic<int> occupied_;
    std::atomic<int> thread_ticket_;
    std::map<int, std::thread> threads_;
    std::set<int> dead_;
    std::mutex mutex_dead_;
    std::mutex mutex_threads_;
};