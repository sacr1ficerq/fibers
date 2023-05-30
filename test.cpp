#include "fiber.cpp"

int f = 0;
void fiber1() {
    printf("Done\n");
    fflush(stdout);
    ++f;
}

void my_test() {
    schedule(fiber1);

    run_global_scheduler();

    assert(f == 1);
}

void test_simple() {
    int x = 0;

    schedule([&]() {
        printf("Done\n");
        fflush(stdout);
        ++x;
    });

    run_global_scheduler();

    assert(x == 1);
}

void test_multiple() {
    int x = 0;

    schedule([&]() {
        ++x;
        std::cout << "Done" << std::endl;
    });
    schedule([&]() {
        ++x;
        std::cout << "Done" << std::endl;
    });
    schedule([&]() {
        ++x;
        std::cout << "Done" << std::endl;
    });

    run_global_scheduler();

    assert(x == 3);
}

void test_recursive() {
    int x = 0;

    schedule([&]() {
        schedule([&]() {
            ++x;
            std::cout << "Done" << std::endl;
        });
    });
    schedule([&]() {
        schedule([&]() {
            schedule([&]() {
                ++x;
                std::cout << "Done" << std::endl;
            });
        });
    });
    schedule([&]() {
        schedule([&]() {
            schedule([&]() {
                schedule([&]() {
                    ++x;
                    std::cout << "Done" << std::endl;
                });
            });
        });
    });

    run_global_scheduler();

    assert(x == 3);
}

void test_yield_one() {
    int x = 0;

    schedule([&]() {
        for (int i = 0; i != ITERS; ++i) {
            ++x;
            yield();
        }
        std::cout << "Done" << std::endl;
    });

    assert(x == 0);

    run_global_scheduler();

    assert(x == ITERS);
}

void test_yield_many() {
    int x = 0;
    int cur_fiber = -1;

    auto create_fiber = [&](int fiber_id) {
        return [&, fiber_id]() {
            for (int i = 0; i != ITERS; ++i) {
                assert(cur_fiber != fiber_id);
                cur_fiber = fiber_id;
                ++x;
                yield();
            }
            std::cout << "Done" << std::endl;
        };
    };

    schedule(create_fiber(1));
    schedule(create_fiber(2));
    schedule(create_fiber(3));

    assert(x == 0);

    run_global_scheduler();

    assert(x == 3 * ITERS);
}

void test_secret() {
    run_global_scheduler();
}
