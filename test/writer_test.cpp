#include "ekutils/writer.hpp"
#include "ekutils/omem_stream.hpp"

#include "test.hpp"

#define TEST_STRING1 "Hello World!!!"
#define TEST_STRING2 "Goodbay World???"

test {
	using namespace ekutils;
	using namespace std::string_literals;
	omem_stream outs;
	writer output(outs);
	output.writeln(TEST_STRING1);
	assert_equals(TEST_STRING1 "\n", std::string(reinterpret_cast<const char *>(outs.data()), outs.size()));
	outs.clear();
	output.writeln(TEST_STRING2 ""s);
	assert_equals(TEST_STRING2 "\n", std::string(reinterpret_cast<const char *>(outs.data()), outs.size()));
}
