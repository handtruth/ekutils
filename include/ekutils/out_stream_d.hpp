#ifndef _OUT_STREAM_D_HEAD
#define _OUT_STREAM_D_HEAD

#include "ekutils/out_stream.hpp"
#include "ekutils/descriptor.hpp"

namespace ekutils {

class out_stream_d : public out_stream, virtual public descriptor {
public:
	virtual int write(const byte_t data[], std::size_t length) override;
};

} // namespace ekutils

#endif // _OUT_STREAM_D_HEAD
