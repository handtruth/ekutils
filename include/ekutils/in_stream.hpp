#ifndef _IN_STREAM_HEAD
#define _IN_STREAM_HEAD

#include <array>

#include <ekutils/primitives.hpp>

namespace ekutils {

struct in_stream {
	virtual int read(byte_t data[], std::size_t length) = 0;
	template <std::size_t N>
	constexpr int read_array(std::array<byte_t, N> & data, const std::size_t length = N) {
		return read(data.data(), length);
	}
	virtual ~in_stream() {}
};

} // namespace ekutils

#endif // _IN_STREAM_HEAD
