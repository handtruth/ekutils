#ifndef _STDERR_D_HEAD
#define _STDERR_D_HEAD

#include <istream>

#include <ekutils/out_stream_d.hpp>

namespace ekutils {

class stderr_d final : public out_stream_d {
public:
	stderr_d();
	stderr_d & operator=(const descriptor & other);
	virtual std::string to_string() const noexcept override;
};

extern stderr_d unerr;

} // namespace ekutils

#endif // _STDIN_D_HEAD
