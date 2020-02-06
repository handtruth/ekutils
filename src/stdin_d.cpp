#include "ekutils/stdin_d.hpp"

#include <unistd.h>

namespace ekutils {

stdin_d::stdin_d() {
	handle = STDIN_FILENO;
}

stdin_d & stdin_d::operator=(const descriptor & other) {
	close();
	dup2(other.fd(), handle = STDIN_FILENO);
	return *this;
}

std::string stdin_d::to_string() const noexcept {
	return "stdin";
}

stdin_d input;

} // namespace ekutils
