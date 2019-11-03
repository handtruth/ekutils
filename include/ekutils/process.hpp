#ifndef _PROCESS_HEAD
#define _PROCESS_HEAD

#include <sys/types.h>
#include <filesystem>
#include <string_view>
#include <functional>

#include <ekutils/pipe_d.hpp>
#include <ekutils/signal_d.hpp>
#include <ekutils/delegate.hpp>

namespace ekutils {

enum class exe_search : std::uint8_t {
	file, path
};

namespace stream {
	enum {
		in = 1, out = 2, err = 4
	};
}

struct process_opts {
	int streams;
	exe_search search = exe_search::path;
};

class process {
	pid_t p_id;
	out_pipe_d std_in;
	in_pipe_d std_out;
	in_pipe_d std_err;
	int status;
	void process_spawn(delegate_base<int(void)> & entrypoint, const process_opts & opts);
public:
	template <typename F>
	process(F & entrypoint, const process_opts & opts = process_opts()) {
		delegate_t<F &, int(void)> task(entrypoint);
		process_spawn(task, opts);
	}
	template <typename F>
	process(F entrypoint, const process_opts & opts = process_opts()) {
		delegate_t<F, int(void)> task(entrypoint);
		process_spawn(task, opts);
	}
	process(
		std::function<int(const std::vector<std::string> &)> & entrypoint,
		const std::vector<std::string> & args = std::vector<std::string>(),
		const process_opts & opts = process_opts());
	process(
		const std::filesystem::path & exe,
		const std::vector<std::string> & args = std::vector<std::string>(),
		const process_opts & opts = process_opts());
	pid_t pid() const noexcept {
		return p_id;
	}
	out_stream_d & stdin_stream() noexcept {
		return std_in;
	}
	in_stream_d & stdout_stream() noexcept {
		return std_out;
	}
	in_stream_d & stderr_stream() noexcept {
		return std_err;
	}
	void kill(sig s);
	int wait();
	bool running();
};

} // namespace ekutils

#endif // _PROCESS_HEAD
