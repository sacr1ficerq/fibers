#include "fiber-scheduler.h"

std::unique_ptr<Scheduler> global_scheduler;

void schedule(Fiber fiber) {
    if (!global_scheduler) {
        throw std::runtime_error("Global scheduler is empty");
    }
    global_scheduler->schedule(std::move(fiber));
}

void yield() {
    if (!global_scheduler) {
        throw std::runtime_error("Global scheduler is empty");
    }
    global_scheduler->yield();
}

void init_global_scheduler() {
    global_scheduler = std::make_unique<FiberScheduler>();
}

void run_global_scheduler() {
    global_scheduler->run();
}