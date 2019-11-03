#ifndef _OMEM_STREAM_HEAD
#define _OMEM_STREAM_HEAD

#include <vector>

#include <ekutils/out_stream.hpp>
#include <ekutils/expandbuff.hpp>

namespace ekutils {

class omem_stream : public out_stream {
	expandbuff buff;
	std::vector<byte_t> collector;
public:
	explicit omem_stream(std::size_t capacity = 0) : buff(capacity) {}
	virtual int write(const byte_t data[], std::size_t length) override;
	constexpr const byte_t * data() const {
		return buff.data();
	}
	constexpr std::size_t size() const {
		return buff.size();
	}
	constexpr void clear() {
		buff.clear();
	}
};

} // namespace ekutils

#endif // _OMEM_STREAM_HEAD
