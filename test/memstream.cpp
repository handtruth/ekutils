#include <array>
#include <cstring>
#include <ekutils/imem_stream.hpp>
#include <ekutils/omem_stream.hpp>

#include "test.hpp"

#include "smth_to_read.hpp"

using namespace ekutils;

test {
    imem_stream input(smth_to_read);
    std::array<byte_t, 100> buffer;

    omem_stream output;
    int r;
    while ((r = input.read_array(buffer)) != -1) {
        output.write_array(buffer, r);
    }
    assert_equals(smth_to_read.size(), output.size());
    std::memcmp(reinterpret_cast<const char *>(smth_to_read.data()),
        reinterpret_cast<const char *>(output.data()), output.size());
}
