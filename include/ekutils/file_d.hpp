#ifndef _FILE_D_HEAD
#define _FILE_D_HEAD

#include <fcntl.h>

#include "ekutils/primitives.hpp"
#include "ekutils/in_stream_d.hpp"
#include "ekutils/out_stream_d.hpp"

namespace ekutils {

class file_d : public in_stream_d, public out_stream_d {
private:
	std::string file;
public:
	enum class mode : int {
		ro = O_RDONLY,
		wo = O_WRONLY,
		rw = O_RDWR,
		aw = O_APPEND,
	};
	file_d(const std::string & path, mode m);
	virtual std::string to_string() const noexcept override;
};

} // namespace ekutils

#endif // _FILE_D_HEAD
