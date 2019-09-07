#include "ekutils/stdin_d.hpp"

namespace ekutils {

std::string stdin_d::to_string() const noexcept {
	return "stdin";
}

stdin_d input;

} // namespace ekutils
