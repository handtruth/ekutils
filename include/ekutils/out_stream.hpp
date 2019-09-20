#ifndef _OUT_STREAM_HEAD
#define _OUT_STREAM_HEAD

#include <array>

#include <ekutils/primitives.hpp>

namespace ekutils {

struct out_stream {
    virtual int write(const byte_t data[], std::size_t length) = 0;
    template <std::size_t N>
    constexpr int write_array(const std::array<byte_t, N> & data, const std::size_t length = N) {
        return write(data.data(), length);
    }
    virtual ~out_stream() {}
};

} // namespace ekutils

#endif // _OUT_STREAM_HEAD
