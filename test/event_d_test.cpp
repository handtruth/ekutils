#include <future>

#include <ekutils/event_d.hpp>

#include "test.hpp"

test {
    using namespace ekutils;

    ekutils::event_d events;

    auto result = std::async(std::launch::async, [&]{
        return events.read();
    });
    events.write(23);
    assert_equals(23u, result.get());

    events.write(42);
    assert_equals(42u, events.read());
}
