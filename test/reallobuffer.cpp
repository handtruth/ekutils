#include "ekutils/reallobuf.hpp"

#include "test.hpp"

test {
    using namespace ekutils;
    reallobuf buff;
    buff.probe(10);
    buff.move(5);
    buff.probe(5);
    buff.data()[9] = 'k';
    assert_equals(10u, buff.size());
}
