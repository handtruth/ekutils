#ifndef _IN_STREAM_HEAD
#define _IN_STREAM_HEAD

#include "ekutils/primitives.hpp"

namespace ekutils {

struct in_stream {
	virtual int read(byte_t data[], std::size_t length) = 0;
	virtual ~in_stream() {}
};

} // namespace ekutils

#endif // _IN_STREAM_HEAD
