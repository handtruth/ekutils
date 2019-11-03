#include "ekutils/process.hpp"

#include <unistd.h>
#include <sys/wait.h>
#include <iostream>

#include <ekutils/stdin_d.hpp>
#include <ekutils/stdout_d.hpp>
#include <ekutils/stderr_d.hpp>

namespace ekutils {

void process::process_spawn(delegate_base<int(void)> & entrypoint, const process_opts & opts) {
	{
		in_pipe_d std_in_pair;
		out_pipe_d std_out_pair;
		out_pipe_d std_err_pair;
		if (opts.streams & stream::in)
			std_in >> std_in_pair;
		if (opts.streams & stream::out)
			std_out << std_out_pair;
		if (opts.streams & stream::err)
			std_err << std_err_pair;
		p_id = fork();
		if (p_id == -1)
			throw std::system_error(std::make_error_code(std::errc(errno)), "failed to spawn process");
		if (p_id)
			return;
		if (opts.streams & stream::in) {
			ekutils::stdin = std_in_pair;
			std_in.close();
		}
		if (opts.streams & stream::out) {
			ekutils::stdout = std_out_pair;
			std_out.close();
		}
		if (opts.streams & stream::err) {
			ekutils::stderr = std_err_pair;
			std_err.close();
		}
	}
	try {
		exit(entrypoint());
	} catch (const std::exception & e) {
		std::cerr << "uncought exception: " << typeid(e).name() << ": " << e.what() << std::endl;
		abort();
	}
}

int exec_entrypoint(const std::filesystem::path & filename,
		const std::vector<std::string> & args, exe_search search) {
	std::size_t argc = args.size();
	typedef char * str;
	str * argv = new str[argc + 2];
	argv[0] = const_cast<char *>(filename.c_str());
	for (std::size_t i = 1; i <= argc; i++) {
		argv[i] = const_cast<char *>(args[i-1].c_str());
	}
	argv[argc + 1] = nullptr;
	switch (search) {
	case exe_search::file:
		execv(filename.c_str(), argv);
		break;
	case exe_search::path:
		execvp(filename.c_str(), argv);
		break;
	}
	perror("process exec failure");
	return 127;
}

process::process(
	std::function<int(const std::vector<std::string> &)> & entrypoint,
	const std::vector<std::string> & args,
	const process_opts & opts) :
		process([&] () -> int {
			return entrypoint(args);
		}, opts) {}

process::process(
	const std::filesystem::path & exe,
	const std::vector<std::string> & args,
	const process_opts & opts) :
		process([&] () -> int {
			return exec_entrypoint(exe, args, opts.search);
		}, opts) {}

void process::kill(sig s) {
	if (::ekutils::kill(p_id, s) == -1)
		throw std::system_error(std::make_error_code(std::errc(errno)),
			"unable to send a signal to process #" + std::to_string(p_id));
}

int process::wait() {
	if (p_id != -1) {
		if (waitpid(p_id, &status, 0) == -1)
			throw std::system_error(std::make_error_code(std::errc(errno)),
				"waiting child process #" + std::to_string(p_id));
		p_id = -1;
	}
	return status;
}

bool process::running() {
	if (p_id == -1)
		return false;
	pid_t result = waitpid(p_id, &status, WNOHANG);
	if (result == 0)
		return true;
	else if (result == -1)
		throw std::system_error(std::make_error_code(std::errc(errno)),
			"checking child process #" + std::to_string(p_id));
	else {
		p_id = -1;
		return false;
	}
}

} // namespace ekutils
