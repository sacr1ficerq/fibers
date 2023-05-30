#pragma once

#include <cassert>
#include <cstring>
#include <iostream>
#include <memory>
#include <queue>
#include <stdexcept>

#include "scheduler.h"

using Fiber = std::function<void()>;

class FiberScheduler : public Scheduler {
    struct Context {
        intptr_t eip = 0;
        intptr_t esp = 0;

        void switch_context();  // TODO
    };

    void run_one();

    struct FiberKeeper;

    static void run_fiber(FiberKeeper fiber);

public:
    ~FiberScheduler() override;

    void schedule(Fiber fiber) override;  // TODO

    void yield() override;  // TODO

    void run() override;  // TODO

    std::queue<Context> queue;

    static Context main_loop;
};

enum {
    ITERS = 10,
};

class InlineScheduler : public Scheduler {
public:
    void schedule(Fiber fiber) override {
        fiber();
    }

    void yield() override {};

    void run() override {};
};