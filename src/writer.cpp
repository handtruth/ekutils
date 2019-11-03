#include "ekutils/writer.hpp"

#include <cstring>

namespace ekutils {

int writer::write(char c) {
	return output.write(reinterpret_cast<const byte_t *>(&c), 1);
}

std::size_t writer::write(const char * string, std::size_t length) {
	return output.write(reinterpret_cast<const byte_t *>(string), length);
}

std::size_t writer::writeln(const char * string, std::size_t length) {
	std::size_t result = write(string, length);
	if (write('\n') == 1)
		return result + 1;
	else
		return result;
}

std::size_t writer::write(const char * string) {
	return output.write(reinterpret_cast<const byte_t *>(string), std::strlen(string));
}

std::size_t writer::writeln(const char * string) {
	std::size_t result = write(string);
	if (write('\n') == 1)
		return result + 1;
	else
		return result;
}

std::size_t writer::write(const std::string & string, std::size_t begin, std::size_t length) {
	std::size_t sz = string.size();
	if (begin > sz)
		throw std::invalid_argument("begin");
	sz -= begin;
	length = (length > sz) ? sz : length;
	return write(string.c_str() + begin, length);
}

std::size_t writer::writeln(const std::string & string, std::size_t begin, std::size_t length) {
	std::size_t result = write(string, begin, length);
	if (write('\n') == 1)
		return result + 1;
	else
		return result;
}

} // namespace ekutils
