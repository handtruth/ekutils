#include "ekutils/pipe_d.hpp"

#include <system_error>
#include <unistd.h>

namespace ekutils {

std::string in_pipe_d::to_string() const noexcept {
	return "input pipe";
}

std::string out_pipe_d::to_string() const noexcept {
	return "output pipe";
}

void pipe(in_pipe_d & input, out_pipe_d & output) {
	int fds[2];
	if (::pipe(fds) == -1)
		throw std::system_error(std::make_error_code(std::errc(errno)), "unable to create pipe");
	input = in_pipe_d(fds[0]);
	output = out_pipe_d(fds[1]);
}

}
