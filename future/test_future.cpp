#include <string>
#include <iostream>
#include <thread>

#include "promise.hpp"
#include "future.hpp"
#include "thread_pool.hpp"
#include "async.hpp"

void task1(stdlike::Promise<int>&& promise)
{
    promise.SetValue(27);
}

void task_exception(stdlike::Promise<int>&& promise)
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

void task_sleeping(stdlike::Promise<int>&& promise)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(900));
    promise.SetValue(27);
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
    std::thread t1(task_exception, std::move(promise));

    try {
        future.Get();
        assert(false);
    } catch(...) {
        assert(true);
    }
    t1.join();
}

void test_tryGet() {
    stdlike::Promise<int> promise;
    auto future = promise.MakeFuture();
    std::thread t1(task_sleeping, std::move(promise));

    assert( future.TryGet() == std::nullopt );
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    assert( future.TryGet().has_value() );
    t1.join();
}

void test_thread_pool() {
    int x = 1;
    ThreadPool tp(2);
    tp.Execute([&x](int y){x = y;}, 27);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    assert( x == 27 );
}

void test_simple_pool() {
    ThreadPool tp(4);
    stdlike::Promise<int> promise;
    auto future = promise.MakeFuture();
    tp.Execute(task1, std::move(promise));

    assert(future.Get() == 27);
}

void test_async() {
    stdlike::Promise<int> promise;
    auto future = promise.MakeFuture();
    async(task1, std::move(promise));

    assert(future.Get() == 27);
}

int mul2( int x) {
    return 2 * x;
}

int add5( int x) {
    return x + 5;
}

void test_simple_then() {
    ThreadPool tp(4);
    stdlike::Promise<int> promise;
    auto future = promise.MakeFuture(&tp);
    tp.Execute([](stdlike::Promise<int> promise) {
        promise.SetValue(3);
    }, std::move(promise));

    auto future_new = future.Then(mul2); // 3 * 2 = 6
    assert(future_new.Get() == 6);
}

void test_then_exception() {
    ThreadPool tp(4);
    stdlike::Promise<int> promise;
    auto future = promise.MakeFuture(&tp);
    tp.Execute(task_exception, std::move(promise));

    auto future_new = future.Then([](int y) {
        assert(false && "Unreachable");
        return 0;
    });
    try {
        future_new.Get();
        assert( false && "Unreachable");
    } catch (...) {
        
    }
}


int main()
{
    test_simple();
    test_exception();
    test_tryGet();
    test_thread_pool();
    test_async();
    test_simple_pool();
    test_simple_then();
    test_then_exception();
}