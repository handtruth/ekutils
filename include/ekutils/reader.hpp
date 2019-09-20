#ifndef _READER_HEAD
#define _READER_HEAD

#include <string>

#include <ekutils/expandbuff.hpp>
#include <ekutils/in_stream.hpp>

namespace ekutils {

class reader {
	expandbuff buff;
	in_stream & input;
	void fetch();
	char getc();
	void gets(std::string & string, std::size_t length);
public:
	explicit reader(in_stream & stream) : input(stream) {}
	char read();
	std::size_t read(std::string & string, std::size_t length);
	bool read_line(std::string & line);
};

} // namespace ekutils

#endif // _READER_HEAD
