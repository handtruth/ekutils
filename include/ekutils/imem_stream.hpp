#ifndef _IMEM_STREAM_HEAD
#define _IMEM_STREAM_HEAD

#include <array>

#include <ekutils/in_stream.hpp>

namespace ekutils {

class imem_stream : public in_stream {
	const byte_t * blob;
	std::size_t size, fetched = 0;
public:
	constexpr imem_stream(const byte_t * data, std::size_t length) :
		blob(data), size(length) {}
	template <std::size_t N>
	constexpr explicit imem_stream(const std::array<byte_t, N> & data) :
		blob(data.data()), size(data.size()) {}
	virtual int read(byte_t data[], std::size_t length) override;
	const byte_t * data() const {
		return blob;
	}
	std::size_t length() const {
		return size;
	}
};

} // namespace ekutils

#endif // _IMEM_STREAM_HEAD
