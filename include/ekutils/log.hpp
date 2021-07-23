
#ifndef _LOG_HEAD
#define _LOG_HEAD

#include <ostream>
#include <string>
#include <exception>
#include <fstream>
#include <mutex>

namespace handtruth::ekutils {

enum class log_level {
	none, fatal, error, warning, info, verbose, debug
};

const std::string & log_lvl2str(log_level lvl);

class log_base {
private:
	log_level lvl;
	std::mutex mutex;
	void write_private(log_level level, const std::string & message);
	void write_exception(log_level level, const std::exception & exception);
protected:
	log_base(log_level level) noexcept : lvl(level) {}
	virtual void write(const std::string & message) = 0;
public:
	void set_log_level(log_level level) noexcept {
		lvl = level;
	}
	log_level log_lvl() const noexcept {
		return lvl;
	}
	void fatal(const std::string & message) {
		write_private(log_level::fatal, message);
	}
	void error(const std::string & message) {
		write_private(log_level::error, message);
	}
	void warning(const std::string & message) {
		write_private(log_level::warning, message);
	}
	void info(const std::string & message) {
		write_private(log_level::info, message);
	}
	void verbose(const std::string & message) {
		write_private(log_level::verbose, message);
	}
	void debug(const std::string & message) {
		write_private(log_level::debug, message);
	}
	void fatal(const std::exception & exception) {
		write_exception(log_level::fatal, exception);
	}
	void error(const std::exception & exception) {
		write_exception(log_level::error, exception);
	}
	void warning(const std::exception & exception) {
		write_exception(log_level::warning, exception);
	}
	void info(const std::exception & exception) {
		write_exception(log_level::info, exception);
	}
	void verbose(const std::exception & exception) {
		write_exception(log_level::verbose, exception);
	}
	void debug(const std::exception & exception) {
		write_exception(log_level::debug, exception);
	}
	virtual ~log_base() {};
};

class file_log : public log_base {
private:
	std::ofstream output;
public:
	file_log(const std::string & filename, log_level level) : log_base(level), output(filename) {
		if (!output) {
			output.close();
			throw std::runtime_error("cannot write to '" + filename + "'");
		}
	}
	virtual void write(const std::string & message) override;
	virtual ~file_log() noexcept override {
		output.close();
	}
};

class stdout_log : public log_base {
public:
	explicit stdout_log(log_level level) : log_base(level) {}
	virtual void write(const std::string & message) override;
};

class empty_log : public log_base {
public:
	empty_log() : log_base(log_level::none) {}
	virtual void write(const std::string &) override {}
};

extern log_base * log;

log_level str2loglvl(const std::string & verb);

class fatal_error : public std::exception {
	std::string m_message;
public:
	explicit fatal_error(const std::string & message) : m_message(message) {}
	explicit fatal_error(const std::exception & other);
	virtual const char * what() const noexcept override {
		return m_message.c_str();
	}
};

#define log_something(lvl, subject) \
		do { \
			if (int(::handtruth::ekutils::log_level::lvl) <= int(::handtruth::ekutils::log->log_lvl())) \
				::handtruth::ekutils::log->lvl(subject); \
		} while (0)

[[noreturn]]
inline int log_fatal_helper(const std::string & subject) {
	log_something(fatal, subject);
	throw ::handtruth::ekutils::fatal_error(subject);
}

[[noreturn]]
inline int log_fatal_helper(const std::exception & subject) {
	log_something(fatal, subject);
	throw ::handtruth::ekutils::fatal_error(subject);
}

} // namespace handtruth::ekutils

#define log_fatal(subject) \
		(throw ::handtruth::ekutils::log_fatal_helper(subject))

#define log_error(subject) \
		log_something(error, subject)

#define log_warning(subject) \
		log_something(warning, subject)

#define log_info(subject) \
		log_something(info, subject)

#define log_verbose(subject) \
		log_something(verbose, subject)

#define log_debug(subject) \
		log_something(debug, subject)

#endif // _LOG_HEAD
