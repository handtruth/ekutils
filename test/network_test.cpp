#include <iostream>

#include "ekutils/tcp_d.hpp"
#include "ekutils/resolver.hpp"

#include "test.hpp"

test {
	using namespace ekutils;

	auto targets = net::resolve(net::socket_types::stream, "tcp://localhost:2375");
	
	assert_false(targets.empty());

	auto server = net::bind_stream(targets.front());
	server->listen();
}
