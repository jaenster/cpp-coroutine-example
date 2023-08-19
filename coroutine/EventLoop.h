#pragma once

#include <list>
#include "Task.h"


class EventLoop {
    std::list<Task::Task<>> tasks;

public:
    EventLoop &operator<<(const std::function<Task::Task<>()> &callback) {
        auto promise = callback();

        this->tasks.push_back(std::move(promise));

        return *this;
    }

    void run() {
        auto i = this->tasks.begin();
        while (i != this->tasks.end()) {
            bool isDone = (*i).resume();
            if (isDone) {
                i = this->tasks.erase(i);
            } else {
                i++;
            }
        }
    }

    void runUntilEmpty() {
        while (!this->tasks.empty())
            this->run();
    }
};
