#ifndef _STDIN_D_HEAD
#define _STDIN_D_HEAD

#include <istream>

#include "ekutils/in_stream_d.hpp"

namespace ekutils {

class stdin_d final : public in_stream_d {
public:
	stdin_d() {
		handle = 0;
	}
	virtual std::string to_string() const noexcept override;
};

extern stdin_d input;

} // namespace ekutils

#endif // _STDIN_D_HEAD
