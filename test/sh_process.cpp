#include "ekutils/process.hpp"

#include "test.hpp"

#define OUT_STR "Hello World!"
#define OUT_STR_SZ (sizeof(OUT_STR))

test {
	using namespace ekutils;
	using namespace std::string_literals;
	process_opts opts { stream::out | stream::err, exe_search::path };
	process sh("sh"s, { "-c"s, "sleep 1 && echo " OUT_STR "; exit 3"s }, opts);
	assert_true(sh.running());
	assert_equals(3 << 8, sh.wait());
	std::array<byte_t, 500> stdout_bytes;
	auto & stdout = sh.stdout();
	assert_equals(int(OUT_STR_SZ), stdout.read_array(stdout_bytes));
	assert_equals(OUT_STR "\n"s, std::string(reinterpret_cast<const char *>(stdout_bytes.data()), OUT_STR_SZ));
	assert_false(sh.running());
}
