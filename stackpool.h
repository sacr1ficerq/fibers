#pragma once

#include "fiber-scheduler.h"

class StackPool {
public:
    enum {
        STACK_SIZE = 1024 * 1024,
    };

    ~StackPool();

    void *alloc();

    void free(void *stack);
private:
    std::vector<void *> stacks;
};
