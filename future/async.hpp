#pragma once

#include "thread_pool.hpp"

static ThreadPool __async_thread_pool(4);

template <typename Function, typename ...Args>
void async( Function&& foo, Args&&... args) {
    __async_thread_pool.Execute( std::move(foo), std::forward<Args>(args)...);
}