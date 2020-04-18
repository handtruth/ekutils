#include "ekutils/mutex_atomic.hpp"
#include "ekutils/putil.hpp"
#include "ekutils/lazy.hpp"
#include "ekutils/lateinit.hpp"

#include <memory>
#include <thread>
#include <future>

#include "test.hpp"

int ohh = 0;

struct base_t {
	~base_t() {};
};

struct hehoh : public base_t {
	~hehoh() {
		ohh = 10;
	}
};

test {
	using namespace ekutils;
	matomic<int> m;
	m = 10;
	assert_equals(10, int(m));
	bool was_called = false;
	{
		finnaly({
			was_called = true;
		});
		assert_equals(false, was_called);
	}

	assert_equals(true, was_called);
	{
		std::shared_ptr<base_t> base = std::make_shared<hehoh>();
	}
	assert_equals(10, ohh);

	lazy<std::string> lvalue {
		[]() -> std::string {
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1s);
			return "test string";
		}
	};
	auto future1 = std::async(std::launch::async, [&lvalue]() -> std::string {
		return lvalue;
	});
	auto future2 = std::async(std::launch::async, [&lvalue]() -> std::string {
		return lvalue;
	});
	auto future3 = std::async(std::launch::async, [&lvalue]() -> std::string {
		return lvalue;
	});
	auto future4 = std::async(std::launch::async, [&lvalue]() -> std::string {
		return lvalue;
	});
	auto future5 = std::async(std::launch::async, [&lvalue]() -> std::string {
		return lvalue;
	});
	assert_equals("test string", lvalue.get());
	assert_equals("test string", future1.get());
	assert_equals("test string", future2.get());
	assert_equals("test string", future3.get());
	assert_equals("test string", future4.get());
	assert_equals("test string", future5.get());

	{
		lateinit<std::string> late;
		assert_false(late.initialized());
		assert_fails_with(std::bad_optional_access, {
			std::string my_val = late;
		});
		late = "initialized";
		assert_true(late.initialized());
		assert_fails_with(std::runtime_error, {
			late = "popka";
		});
		assert_equals("initialized", late.get());
	}
}
