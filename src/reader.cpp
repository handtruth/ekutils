#include "ekutils/reader.hpp"

#include <cstring>
#include <cassert>

#include <iostream>

namespace ekutils {

const std::size_t part = 256;

void reader::fetch() {
	int s = 0;
	do {
		buff.asize(part);
		s = input.read(buff.data() + buff.size() - part, part);
		if (s == -1) {
			buff.ssize(part);
			return;
		}
		buff.ssize(part - s);
	} while (s == part);
}

char reader::getc() {
	if (buff.size()) {
		char c = *buff.data();
		buff.move(1);
		return c;
	}
	return -1;
}

void reader::gets(std::string & string, std::size_t length) {
	string.append(reinterpret_cast<const char *>(buff.data()), length);
	buff.move(length);
}

char reader::read() {
	char r = getc();
	if (r != -1)
		return r;
	fetch();
	r = getc();
	if (r != -1)
		return r;
	return -1;
}

std::size_t reader::read(std::string & string, std::size_t length) {
	if (buff.size() < length)
		fetch();
	std::size_t toread = std::min(buff.size(), length);
	gets(string, toread);
	return toread;
}

bool reader::readln(std::string & line) {
	std::size_t i;
	auto ptr = buff.data();
	for (i = 0; i < buff.size(); i++) {
		if (ptr[i] == '\n') {
			gets(line, i);
			char c = getc();
			assert(c == '\n');
			return true;
		}
	}
	fetch();
	ptr = buff.data();
	for (; i < buff.size(); i++) {
		if (ptr[i] == '\n') {
			gets(line, i);
			char c = getc();
			assert(c == '\n');
			return true;
		}
	}
	return false;
}

} // namespace ekutils
