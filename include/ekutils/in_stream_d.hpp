#ifndef _IN_STREAM_D_HEAD
#define _IN_STREAM_D_HEAD

#include "ekutils/in_stream.hpp"
#include "ekutils/descriptor.hpp"

namespace ekutils {

class in_stream_d : public in_stream, virtual public descriptor {
public:
	virtual int read(byte_t data[], std::size_t length) override;
};

} // namespace ekutils

#endif // _IN_STREAM_D_HEAD
