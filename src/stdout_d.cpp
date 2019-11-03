#include "ekutils/stdout_d.hpp"

#include <unistd.h>

namespace ekutils {

stdout_d::stdout_d() {
	handle = STDOUT_FILENO;
}

stdout_d & stdout_d::operator=(const descriptor & other) {
	close();
	dup2(other.fd(), handle = STDOUT_FILENO);
	return *this;
}

std::string stdout_d::to_string() const noexcept {
	return "stdout";
}

stdout_d stdout;

} // namespace ekutils
