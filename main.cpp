#include "test.cpp"

int main() {
    init_global_scheduler();

    my_test();
    test_simple();
    test_multiple();
    test_recursive();
    test_yield_one();
    test_yield_many();
    test_secret();
}