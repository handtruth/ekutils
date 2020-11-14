#include <iostream>

#include "ekutils/arguments.hpp"

#include "test.hpp"

struct testargs : public ekutils::arguments {
	std::string & test_str;
	bool & test_flag;
	bool & other_flag;
	std::intmax_t & some_int;
	std::intmax_t & desc_int;
	std::intmax_t & long_name;
	double & number;
	multistring::value_type & multi;

	enum class test_enum {
		ONE, TWO, THREE
	};

	test_enum & variant_test;

	testargs() : test_str(add<string>("test-str", [](string & opt) {
		opt.hint = "testificate string argument";
	})),
	test_flag(add<flag>("test-flag", [](flag & opt) {
		opt.c = 'f';
		opt.hint = "some flag with short form";
	})),
	other_flag(add<flag>("other-flag", [](flag & opt) {
		opt.hint = "some flag without short form";
	})),
	some_int(add<integer>("some-int", [](integer & opt) {
		opt.min = 30;
		opt.max = 56;
		opt.hint = "an integer";
	})),
	desc_int(add<integer>("desc-int", [](integer & opt) {
		opt.min = -23;
		opt.hint = "a very long description of a simple property/option/argument value that does nothing in purpose of the test case that you can see here";
	})),
	long_name(add<integer>("long-name", [](integer & opt) {
		opt.max = 165946532;
		opt.hint = "aaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccdddddddddddddddddddddd";
	})),
	number(add<fractional>("number")),
	multi(add<multistring>("multi", [](multistring & opt) {
		opt.hint = "multistring test";
	})),
	variant_test(add<variant<test_enum>>("vards", [](variant<test_enum> & opt) {
		opt.variants = {
			{ "one", test_enum::ONE },
			{ "two", test_enum::TWO },
			{ "three", test_enum::THREE }
		};
	}))
	{}
};

test {
	testargs args;
	const char * tc[] {
		"./program", "--test-str=a value", "-f", "--desc-int", "3568", "--multi=hello",
		"--multi", "world", "positional", "--vards=two", "--", "--not_flag"
	};
	std::cout << args.build_help("lol") << std::endl;
	args.parse(12, tc);
	assert_equals("a value", args.test_str);
	assert_equals(true, args.test_flag);
	assert_equals(false, args.other_flag);
	assert_equals(3568, args.desc_int);
	assert_equals(2u, args.multi.size());
	assert_equals("hello", args.multi.front());
	assert_equals("world", args.multi[1]);
	assert_equals(2u, args.positional.size());
	assert_equals("positional", args.positional.front());
	assert_equals("--not_flag", args.positional.at(1));
	assert_equals(int(testargs::test_enum::TWO), int(args.variant_test));
}
