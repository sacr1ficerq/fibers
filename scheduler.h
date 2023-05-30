#pragma once

#include <functional>

using Fiber = std::function<void()>;

class Scheduler {
public:
    virtual ~Scheduler() = default;

    virtual void schedule(Fiber fiber) = 0;

    virtual void yield() = 0;

    virtual void run() = 0;
};
