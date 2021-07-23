#include "ekutils/parse_essentials.hpp"

#include <stdexcept>

namespace handtruth::ekutils {

std::size_t find_escaped_end(const char * str) {
	std::size_t length = 0;
	char c = *str;
	while (true) {
		if (c == '\0')
			throw std::runtime_error("end of string");
		if (c == '"')
			break;
		if (c == '\\' && str[length + 1] == '"')
			++length;
		c = str[++length];
	}
	return length + 1;
}

std::string unescape(const char * str, std::size_t length) {
	std::string result;
	result.reserve(length);
	for (std::size_t i = 0; i < length; i++) {
		if (str[i] == '\\') {
			switch (str[i + 1]) {
			case 'n':
				i++;
				result.push_back('\n');
				break;
			case '"':
				i++;
				result.push_back('"');
				break;
			case '\'':
				i++;
				result.push_back('\'');
				break;
			case '\\':
				i++;
				result.push_back('\\');
				break;
			case 't':
				i++;
				result.push_back('\t');
				break;
			case '0':
				i++;
				result.push_back('\0');
				break;
			case 'r':
				i++;
				result.push_back('\r');
				break;
			default:
				result.push_back('\\');
				break;
			}
		} else {
			result.push_back(str[i]);
		}
	}
	return result;
}

} // namespace handtruth::ekutils
