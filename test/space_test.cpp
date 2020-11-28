#include <iostream>

#include "ekutils/space.hpp"
#include "ekutils/ip_space.hpp"
#include "ekutils/socket_d.hpp"

#include "test.hpp"

template <typename T>
void dump_space(const T & space) {
	for (const auto & range : space.ranges())
		std::cout << '[' << std::to_string(range.first) << '-' << std::to_string(range.second) << "]; ";
	std::cout << std::endl;
}

test {
	namespace ipv4 = ekutils::net::ipv4;
	namespace ipv6 = ekutils::net::ipv6;

	ekutils::space<int> intspace;
	assert_true(intspace.add(3));
	assert_false(intspace.add(3));
	assert_equals(1u, intspace.ranges().size());
	assert_true(intspace.add(4));
	assert_false(intspace.add(4));
	assert_equals(1u, intspace.ranges().size());
	assert_true(intspace.add(6));
	assert_false(intspace.add(6));
	assert_equals(2u, intspace.ranges().size());
	assert_true(intspace.add(7));
	assert_false(intspace.add(7));
	assert_equals(2u, intspace.ranges().size());
	assert_true(intspace.add(5));
	assert_false(intspace.add(5));
	assert_equals(1u, intspace.ranges().size());
	assert_true(intspace.add(1));
	assert_false(intspace.add(1));
	assert_equals(2u, intspace.ranges().size());
	assert_true(intspace.add(2));
	assert_false(intspace.add(2));
	assert_equals(1u, intspace.ranges().size());
	assert_true(intspace.add(30));
	assert_false(intspace.add(30));
	assert_equals(2u, intspace.ranges().size());
	assert_true(intspace.add(8));
	assert_false(intspace.add(8));
	assert_equals(2u, intspace.ranges().size());
	assert_true(intspace.add(29));
	assert_false(intspace.add(29));
	assert_equals(2u, intspace.ranges().size());
	assert_true(intspace.add(20));
	assert_false(intspace.add(20));
	assert_equals(3u, intspace.ranges().size());
	assert_true(intspace.add(0));
	assert_false(intspace.add(0));
	assert_equals(3u, intspace.ranges().size());
	dump_space(intspace);

	assert_false(intspace.contains(9));
	assert_false(intspace.contains(-3));
	assert_false(intspace.contains(31));
	assert_false(intspace.contains(21));
	assert_true(intspace.contains(5));
	assert_true(intspace.contains(30));
	assert_true(intspace.contains(0));
	assert_true(intspace.contains(20));


	// Ranges
	assert_false(intspace.add(5, 7));
	assert_false(intspace.add(0, 8));
	assert_false(intspace.add(20, 20));
	assert_false(intspace.add(30, 30));
	assert_false(intspace.add(6, 8));
	assert_false(intspace.add(0, 3));

	assert_true(intspace.add(-1, 9));
	dump_space(intspace);
	assert_true(intspace.add(-2, 9));
	dump_space(intspace);
	assert_true(intspace.add(-10, 9));
	assert_true(intspace.add(-10, 12));
	assert_true(intspace.add(-6, 13));
	assert_equals(3u, intspace.ranges().size());
	assert_true(intspace.add(-6, 29));
	assert_equals(1u, intspace.ranges().size());
	dump_space(intspace);
	assert_true(intspace.add(-6, 50));
	assert_equals(1u, intspace.ranges().size());

	dump_space(intspace);

	// ipv4 space

	{
		ipv4::space ip4space;

		std::pair range1 = ipv4::parse_network("127.0.0.0/23");
		std::pair range2 = ipv4::parse_network("10.1.0.40-10.1.0.30");
		std::pair range3 = ipv4::parse_network("1.2.3.4");
		ip4space.insert(range1.first, range1.second);
		ip4space.insert(range2.first, range2.second);
		dump_space(ip4space);
		ip4space.insert(range3.first, range3.second);
		dump_space(ip4space);

		assert_true(ip4space.contains(ipv4::address("127.0.0.1")));
		assert_true(ip4space.contains(ipv4::address("10.1.0.33")));
		assert_true(ip4space.contains(ipv4::address("1.2.3.4")));
		assert_false(ip4space.contains(ipv4::address("10.1.0.41")));
	}

	// ipv6 space

	{
		ipv6::space ip6space;

		std::pair range1 = ipv6::parse_network("FF02:0:0:0:0:1:FF00::/102");
		std::pair range2 = ipv6::parse_network("2001:0DB8::ABCD:0:0:1234-2001:0DB8::ABCD:0:1:0");
		std::pair range3 = ipv6::parse_network("02:42:ac:11::0:3");
		ip6space.insert(range1.first, range1.second);
		ip6space.insert(range2.first, range2.second);
		dump_space(ip6space);
		ip6space.insert(range3.first, range3.second);
		dump_space(ip6space);

		assert_equals(0xfu, ipv6::address("F00::").data[0]);
		assert_true(ipv6::address("::F") < ipv6::address("F::"));

		assert_true(ip6space.contains(ipv6::address("FF02:0:0:0:0:1:FF00::")));
		assert_true(ip6space.contains(ipv6::address("2001:0DB8::ABCD:0:0:FF00")));
		assert_true(ip6space.contains(ipv6::address("02:42:ac:11::0:3")));
		assert_false(ip6space.contains(ipv6::address("2001:0DB8::ABCD:0:2:1")));
	}

	// ip space

	{
		ekutils::net::addresses addresses;

		assert_true(addresses.empty());

		addresses.add("*lo");
		std::cout << "BEGIN INTERFACES:" << std::endl;
		dump_space(addresses.ip6space);
		dump_space(addresses.ip4space);
		std::cout << "END INTERFACES." << std::endl;
		assert_false(addresses.empty());
		addresses.add("10.1.0.40-10.1.0.30");
		addresses.add("FF02:0:0:0:0:1:FF00::/102");

		dump_space(addresses.ip6space);
		dump_space(addresses.ip4space);

		assert_true(addresses.contains(ipv6::address("FF02:0:0:0:0:1:FF00::")));
		assert_true(addresses.contains(ipv4::address("127.0.0.1")));
		assert_true(addresses.contains(ipv4::address("10.1.0.33")));
	}
}
