#include "test.hpp"

#include "ekutils/callable.hpp"

test {
    using namespace handtruth::ekutils;
    int c = 0;
    delegate<int(int, int)> adder = [&c](auto a, auto b) -> auto { return a + b + c; };
    delegate other = std::move(adder);
    c = 3;
    assert_equals(7, other(2, 2));
}
