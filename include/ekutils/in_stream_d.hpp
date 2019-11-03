#ifndef _IN_STREAM_D_HEAD
#define _IN_STREAM_D_HEAD

#include <ekutils/in_stream.hpp>
#include <ekutils/descriptor.hpp>

namespace ekutils {

class in_stream_d : public in_stream, virtual public descriptor {
public:
	in_stream_d() = default;
	virtual int read(byte_t data[], std::size_t length) override;
protected:
	explicit in_stream_d(handle_t fd) : descriptor(fd) {}
	in_stream_d(in_stream_d && other) : descriptor(std::move(other)) {}
};

} // namespace ekutils

#endif // _IN_STREAM_D_HEAD
