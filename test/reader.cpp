#include <cstring>
#include <ekutils/reader.hpp>
#include <ekutils/imem_stream.hpp>

#include "test.hpp"

#include "smth_to_read.hpp"

using namespace ekutils;

test {
    std::cout << "STACK: " << (void *)smth_to_read.data() << std::endl;
    imem_stream input(smth_to_read);
    reader testificate(input);

    std::string line;
    assert_true(testificate.read_line(line));
    assert_equals("## Minecraft server hub configuration file", line);
    line.clear();
    assert_equals('\n', testificate.read());
    assert_true(testificate.read_line(line));
    assert_equals("## All options marked as dynamic can be changed while MCSHub is working.", line);
}
