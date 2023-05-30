#include "stackpool.h"

StackPool::~StackPool() {
    for (auto elem : stacks) {
        ::free(elem);
    }
}

void *StackPool::alloc() {
    if (!stacks.empty()) {
        auto ptr = stacks.back();
        stacks.pop_back();
        return ptr;
    }
    auto ptr = calloc(1, STACK_SIZE);
    return ptr;
}

void StackPool::free(void *stack) {
    stacks.push_back(stack);
}
