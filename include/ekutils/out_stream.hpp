#ifndef _OUT_STREAM_HEAD
#define _OUT_STREAM_HEAD

#include "ekutils/primitives.hpp"

namespace ekutils {

struct out_stream {
    virtual int write(const byte_t data[], std::size_t length) = 0;
    virtual ~out_stream() {}
};

} // namespace ekutils

#endif // _OUT_STREAM_HEAD
