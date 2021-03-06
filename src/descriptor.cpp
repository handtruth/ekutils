#include "ekutils/descriptor.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <system_error>
#include <cerrno>
#include <sys/ioctl.h>

namespace ekutils {

void descriptor::set_non_block(bool nonblock) {
	int flags = fcntl(handle, F_GETFL);
	if (flags == -1) {
		throw std::system_error(std::make_error_code(std::errc(errno)), "error while getting flags of fd");
	}
	if (nonblock)
		flags |= O_NONBLOCK;
	else
		flags &= ~O_NONBLOCK;
	flags = fcntl(handle, F_SETFL, flags);
	if (flags == -1) {
		throw std::system_error(std::make_error_code(std::errc(errno)),
			"error while setting non blocking state to fd");
	}
}

std::size_t descriptor::avail() const {
	std::size_t size = 0;
	if (ioctl(handle, FIONREAD, &size) == -1)
		throw std::system_error(std::make_error_code(std::errc(errno)),
            "error while getting available bytes");
	return size;
}

void descriptor::close() {
	if (*this) {
		::close(handle);
		handle = -1;
	}
}

descriptor::record_base::~record_base() {}

descriptor::~descriptor() {
	close();
}

void descriptor::check_created() const {
	if (!*this)
		throw std::runtime_error("file descriptor not created");
}

} // namespace ekutils
