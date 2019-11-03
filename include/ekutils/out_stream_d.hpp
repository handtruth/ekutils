#ifndef _OUT_STREAM_D_HEAD
#define _OUT_STREAM_D_HEAD

#include <ekutils/out_stream.hpp>
#include <ekutils/descriptor.hpp>

namespace ekutils {

class out_stream_d : public out_stream, virtual public descriptor {
public:
	out_stream_d() = default;
	virtual int write(const byte_t data[], std::size_t length) override;
protected:
	explicit out_stream_d(handle_t fd) : descriptor(fd) {}
	out_stream_d(out_stream_d && other) : descriptor(std::move(other)) {}
};

} // namespace ekutils

#endif // _OUT_STREAM_D_HEAD
