#include "ekutils/log.hpp"

#include <array>
#include <typeinfo>
#include <iostream>
#include <limits>

namespace ekutils {

const std::array<const std::string, (int)log_level::debug + 1> lvl_strs {
	"none", "fatal", "error", "warning", "info", "verbose", "debug"
};

const std::string & log_lvl2str(log_level lvl) {
	return lvl_strs[(int)lvl];
}

void log_base::write_private(log_level level, const std::string & message) {
	std::lock_guard<std::mutex> lock(mutex);
	if ((int)level > (int)lvl)
		return;
	write("[" + lvl_strs[(int)level] + "]:" + ' ' + message);
}

void log_base::write_exception(log_level level, const std::exception & exception) {
	std::lock_guard<std::mutex> lock(mutex);
	if ((int)level > (int)lvl)
		return;
	write("[" + lvl_strs[(int)level] + "]:" + ' ' + typeid(exception).name() + ": " + exception.what());
}

void file_log::write(const std::string & message) {
	output << message << std::endl;
}

void stdout_log::write(const std::string & message) {
	std::cout << message << std::endl;
}

empty_log no_log;

log_base * log = &no_log;

log_level str2loglvl(const std::string & verb) {
	if (verb == "none")
		return log_level::none;
	if (verb == "fatal")
		return log_level::fatal;
	if (verb == "error")
		return log_level::error;
	if (verb == "warning")
		return log_level::warning;
	if (verb == "info")
		return log_level::info;
	if (verb == "verbose")
		return log_level::verbose;
	if (verb == "debug")
		return log_level::debug;
	if (std::isdigit(verb[0])) {
		int n = std::stoi(verb);
		return log_level(n);
	}
	throw std::runtime_error("no such log level: " + verb);
}

fatal_error::fatal_error(const std::exception & other) :
m_message(typeid(other).name() + std::string(": ") + other.what()) {}

} // namespace ekutils
