#ifndef _EKUTILS_WRITER_HEAD
#define _EKUTILS_WRITER_HEAD

#include <limits>

#include <ekutils/out_stream.hpp>

namespace ekutils {

class writer {
	out_stream & output;
public:
	writer(const writer & other) = delete;
	explicit writer(out_stream & stream) : output(stream) {}
	int write(char c);
	std::size_t write(const char * string, std::size_t length);
	std::size_t writeln(const char * string, std::size_t length);
	std::size_t write(const char * string);
	std::size_t writeln(const char * string);
	std::size_t write(const std::string & string,
		std::size_t begin = 0, std::size_t length = std::numeric_limits<std::size_t>::max());
	std::size_t writeln(const std::string & line,
		std::size_t begin = 0, std::size_t length = std::numeric_limits<std::size_t>::max());
};

} // namespace ekutils

#endif // _EKUTILS_WRITER_HEAD
