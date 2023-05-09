#include "macro_exceptions.hpp"
#include <assert.h>

int global_val = 0;

class Test: public DestructableObject {
public:
    Test() {}
    ~Test() override {
        // std::cout << "Destroyed\n";
        ++global_val;
    }
};


void test_basic() {
    int val = 0;
    TRY {
        val = 1;
        THROW(io_error);
    } CATCH(io_error) {
        val = 2;
    }
    assert( val == 2);
}

void test_types() {
    int val = 0;
    TRY {
        val = 1;
        THROW(io_error);
    } CATCH(bad_alloc) {
        val = 2;
    } CATCH(io_error) {
        val = 3;
    }
    assert( val == 3);
}

void test_all() {
    int val = 0;
    TRY {
        val = 1;
        THROW(error_t::bad_weak_ptr);
    } CATCH(bad_alloc) {
        val = 2;
    } CATCH(io_error) {
        val = 3;
    } CATCH_ALL(...) {
        val = 4;
    }
    assert( val == 4);
}

void exception_thrower(error_t exc) {
    THROW(exc);
}

void test_nested() {
    int val = 0;
    TRY {
        exception_thrower(error_t::invalid_argument);
    } CATCH_ALL(...) {
        val = 1;
    }
    assert( val == 1);
}

void test_destructors() {
    TRY {
        Test test;
        exception_thrower(error_t::invalid_argument);
    } CATCH_ALL(...) {
        ++global_val;
    }
    assert( global_val == 2);
    global_val = 0;
}

void test_destructors_nested() {
    TRY {
        Test test;
        TRY {
            Test test;
            exception_thrower(error_t::invalid_argument);
        } CATCH_ALL(...) {
            ++global_val;
            assert( global_val == 2);
            THROW(error_t::bad_weak_ptr);
        }
    } CATCH_ALL(...) {
        ++global_val;
    }
    assert( global_val == 4);
    global_val = 0;
}

void test_further() {
    TRY {
        TRY {
            exception_thrower(error_t::invalid_argument);
        } CATCH_ALL(...) {
            ++global_val;
            assert( global_val == 1);
            THROW_FURTHER;
        }
    } CATCH_ALL(...) {
        ++global_val;
        assert( global_val == 2);
    }
    assert( global_val == 2);
    global_val = 0;
}

int main() {
    test_basic();
    test_types();
    test_all();
    test_nested();
    test_destructors();
    test_destructors_nested();
    test_further();
}