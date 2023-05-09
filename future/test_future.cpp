#include <string>
#include <iostream>
#include <thread>

#include "promise.hpp"

void task1(stdlike::Promise<int>&& promise)
{
    promise.SetValue(27);
}

void task2(stdlike::Promise<int>&& promise)
{
    std::exception_ptr eptr;
    try
    {
        std::string().at(1);
        promise.SetValue(42);
    }
    catch(...)
    {
        promise.SetException(std::current_exception());
    }
}

void test_simple() {
    stdlike::Promise<int> promise;
    auto future = promise.MakeFuture();
    std::thread t1(task1, std::move(promise));

    assert(future.Get() == 27);
    t1.join();
}

void test_exception() {
    stdlike::Promise<int> promise;
    auto future = promise.MakeFuture();
    std::thread t1(task2, std::move(promise));

    try {
        future.Get();
        assert(false);
    } catch(...) {
        assert(true);
    }
    t1.join();
}

int main()
{
    test_simple();
    test_exception();
}