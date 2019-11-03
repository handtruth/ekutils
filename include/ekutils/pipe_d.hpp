#ifndef _PIPE_D_HEAD
#define _PIPE_D_HEAD

#include <utility>

#include <ekutils/in_stream_d.hpp>
#include <ekutils/out_stream_d.hpp>

namespace ekutils {

class in_pipe_d;
class out_pipe_d;

class in_pipe_d final : public in_stream_d {
	explicit in_pipe_d(handle_t fd) { handle = fd; }
public:
	in_pipe_d() {}
	in_pipe_d(in_pipe_d && other) : in_stream_d(std::move(other)) {}
	in_pipe_d & operator=(in_pipe_d && other) {
		descriptor::operator=(std::move(other));
		return *this;
	}
	virtual std::string to_string() const noexcept override;
	virtual ~in_pipe_d() override {}
friend void pipe(in_pipe_d & input, out_pipe_d & output);
};

class out_pipe_d final : public out_stream_d {
	explicit out_pipe_d(handle_t fd) { handle = fd; }
public:
	out_pipe_d() {}
	out_pipe_d(out_pipe_d && other) : out_stream_d(std::move(other)) {}
	out_pipe_d & operator=(out_pipe_d && other) {
		descriptor::operator=(std::move(other));
		return *this;
	}
	virtual std::string to_string() const noexcept override;
	virtual ~out_pipe_d() override {}
friend void pipe(in_pipe_d & input, out_pipe_d & output);
};

void pipe(in_pipe_d & input, out_pipe_d & output);

inline void operator>>(out_pipe_d & output, in_pipe_d & input) {
	pipe(input, output);
}

inline void operator<<(in_pipe_d & input, out_pipe_d & output) {
	pipe(input, output);
}

} // namespace ekutils

#endif // _PIPE_D_HEAD
