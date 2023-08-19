#include <iostream>
#include <cassert>
#include "coroutine/Task.h"
#include "coroutine/EventLoop.h"

std::thread::id mainTreadId;

Task::Task<> loop1() {
    std::cout << "Start loop1" << "\n";
    for (int i = 0; i < 10; i++) {
        assert(std::this_thread::get_id() == mainTreadId);

        std::cout << "--#1: " << i << "\n";
        co_await Task::delay(std::chrono::milliseconds(15));
    }
    std::cout << "End loop1" << "\n";
}

Task::Task<int> loop2() {
    std::cout << "Start loop2" << "\n";
    int i = 0;
    for (; i < 10; i++) {
        assert(std::this_thread::get_id() == mainTreadId);
        std::cout << "--#2: " << i << "\n";
        co_await Task::delay(std::chrono::milliseconds(20));
    }
    std::cout << "End loop2" << "\n";

    co_return i;
}

Task::Task<> awaitLoop2() {
    std::cout << "await for loop 2" << "\n";
    int result = co_await loop2();
    assert(std::this_thread::get_id() == mainTreadId);
    std::cout << "awaiting loop 2 done. Result:" << result << "\n";
}

int main() {
    mainTreadId = std::this_thread::get_id();

    EventLoop worker;

    worker << loop1;
    worker << awaitLoop2;
    worker << []() -> Task::Task<> {
        std::cout << "Start loop3" << "\n";
        for (int i = 0; i < 10; i++) {
            assert(std::this_thread::get_id() == mainTreadId);
            std::cout << "--#3:" << i << "\n";
            co_await Task::delay(std::chrono::milliseconds(25));
        }
        std::cout << "End loop3" << "\n";
    };

    worker.runUntilEmpty();

    return 0;
}