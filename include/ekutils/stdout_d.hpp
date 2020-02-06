#ifndef _STDOUT_D_HEAD
#define _STDOUT_D_HEAD

#include <istream>

#include <ekutils/out_stream_d.hpp>

namespace ekutils {

class stdout_d final : public out_stream_d {
public:
	stdout_d();
	stdout_d & operator=(const descriptor & other);
	virtual std::string to_string() const noexcept override;
};

extern stdout_d output;

} // namespace ekutils

#endif // _STDOUT_D_HEAD
