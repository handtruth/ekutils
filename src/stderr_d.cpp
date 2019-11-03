#include "ekutils/stderr_d.hpp"

#include <unistd.h>

namespace ekutils {

stderr_d::stderr_d() {
	handle = STDERR_FILENO;
}

stderr_d & stderr_d::operator=(const descriptor & other) {
	close();
	dup2(other.fd(), handle = STDERR_FILENO);
	return *this;
}

std::string stderr_d::to_string() const noexcept {
	return "stderr";
}

stderr_d stderr;

} // namespace ekutils
