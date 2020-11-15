#include "sys_error.hpp"

#include <system_error>

namespace ekutils {

void sys_error(const std::string & message) {
	throw std::system_error(std::make_error_code(std::errc(errno)), message);
}

} // namespace ekutils
