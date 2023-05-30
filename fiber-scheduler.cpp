#include "fiber-scheduler.h"
#include "stackpool.h"

StackPool stack_pool;

FiberScheduler::Context FiberScheduler::main_loop;

void FiberScheduler::run_one() {
    assert(!queue.empty());

    main_loop = queue.front();
    queue.pop();

    main_loop.switch_context();
}

struct FiberScheduler::FiberKeeper {
    Fiber fiber;
    void *stack;

    explicit FiberKeeper(Fiber fiber)
            : fiber(std::move(fiber)), stack(stack_pool.alloc()) {
    }

    FiberKeeper(FiberKeeper &&other)  noexcept : fiber(std::move(other.fiber)) {
        stack = other.stack;
        other.stack = nullptr;
    }

    FiberKeeper(const FiberKeeper &other) = delete;
    void operator=(FiberKeeper &&other) = delete;
    void operator=(const FiberKeeper &other) = delete;

    ~FiberKeeper() {
        if (stack) {
            stack_pool.free(stack);
        }
    }
};

void FiberScheduler::run_fiber(FiberKeeper fiber) {
//    printf("run_fiber()\n");
//    fflush(stdout);

    fiber.fiber();

    printf("fiber_finished...\n");
    fflush(stdout);

    fiber.~FiberKeeper();

    printf("destruction finished...\n");
    fflush(stdout);

    main_loop.switch_context(); // returning to ret_label
    __builtin_unreachable();
}

FiberScheduler::~FiberScheduler() {
    assert(queue.empty());
}

void push(intptr_t &stack_ptr, void *value, size_t size) {
    std::memcpy((void *)(stack_ptr - size), value, size); // put fk on the stack
    stack_ptr -= size;
}

void FiberScheduler::schedule(Fiber fiber){
    auto fk = FiberKeeper(std::move(fiber));
    auto context = Context();

    context.eip = (intptr_t)(&run_fiber);

    // push fk to stack
    intptr_t stack_ptr = ((intptr_t)fk.stack + StackPool::STACK_SIZE);
    push(stack_ptr, (void *)&fk, sizeof(fk));

    auto fk_ptr = (struct FiberKeeper*)stack_ptr;
    push(stack_ptr, (void *)&fk_ptr, sizeof(fk_ptr));

//    fk.stack = (void *)((intptr_t)fk.stack + StackPool::STACK_SIZE);
//    auto fk_pos = (void *)((intptr_t)fk.stack - sizeof(fk));
//    auto fk_ptr = (struct FiberKeeper*)fk_pos;
//    auto ptr_pos = (void *)((intptr_t)fk.stack - sizeof(fk) - sizeof(&fk_ptr));
//
//    std::memcpy(fk_pos, &fk, sizeof(fk)); // put fk on the stack
//    std::memcpy(ptr_pos, &fk_ptr, sizeof(fk_ptr));  // put pointer last on stack
//    context.esp = (intptr_t)ptr_pos;
    context.esp = (intptr_t)stack_ptr;

    std::memset((void *)&fk, 0, sizeof(fk)); // clear fk

    queue.push(context);
} // TODO (DONE)

void FiberScheduler::yield() {
    printf("yield()\n");
    fflush(stdout);

    queue.push(main_loop);

    queue.back().switch_context(); // switch to scheduler context

    printf("returned to yield()\n");
    fflush(stdout);
}  // TODO

int fiber_no = 0;
void FiberScheduler::run(){
    while (!queue.empty()) {
        printf("running: %d\n", fiber_no++);
        run_one();
    }
} // TODO (DONE)

void FiberScheduler::Context::switch_context() {
    printf("switch_context()\n");
    fflush(stdout);
    /*
     * 1) this.eip to ret_label
     * 2) swap this.esp and %%esp
     * 3) jmp this.eip before return_label
     * */

    auto *eipp = &eip;
    auto *espp = &esp;

    asm volatile(
//            "movl %%esp, %%ebp\n"          // save esp
            "pushl %%ebp\n"                // save old ebp

            "movl %%esp, %%eax\n"          // store current esp in eax
            "movl (%0), %%esp\n"           // set esp to this.esp
            "movl %%eax, (%0)\n"           // save current esp in context.esp


            "movl (%1), %%ecx\n"      // switch eip with ret_label

            "movl $ret_label, %%eax\n"      // switch eip with ret_label
            "movl %%eax, (%1)\n"           // switch eip with ret_label

            "call *%%ecx\n"                 // JMP!!!

            "ret_label:\n"                 // here we get after we return from run_fiber()

            "popl %%eax\n"                 // pop ra
            "popl %%ebp\n"                 // load old ebp

            : "+S" (espp), "+D" (eipp)     // store &context.esp in esi and &context.eip in edi
    :: "eax", "ebx", "ecx", "edx", "memory", "cc"); // restoring everything afterwards
    // now eipp, essp point to current context, so if we jump into it, we get here with current state
    printf("returned to switch_context()\n");
    fflush(stdout);

    // we can save it to main_loop and jmp to $eip?
} // TODO
