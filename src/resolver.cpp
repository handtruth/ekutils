#include "ekutils/resolver.hpp"

#include <cstring>
#include <netdb.h>

#include "ekutils/tcp_d.hpp"
#include "ekutils/udp_d.hpp"
#include "ekutils/unix_d.hpp"
#include "ekutils/finally.hpp"

namespace ekutils::net {

std::forward_list<resolution> resolve(const std::string & host, const std::string & port, socket_types type, protocols protocol) {
	addrinfo initial, *sysaddr = nullptr, *a = nullptr;
	std::memset(&initial, 0, sizeof(addrinfo));
	initial.ai_family = AF_UNSPEC;
	initial.ai_socktype = 0;//int(type);
	initial.ai_protocol = int(protocol);
	
	finally({
		if (sysaddr != nullptr)
			freeaddrinfo(sysaddr);
	});

	if (int s = getaddrinfo(host.c_str(), port.c_str(), &initial, &sysaddr)) {
		throw resolver_error("resolution failure \"" + host + ':' + port + "\" (getaddrinfo: " + std::string(gai_strerror(s)) + ")");
	}
	std::forward_list<resolution> result;
	auto placer = result.before_begin();
	for (a = sysaddr; a != nullptr; a = a->ai_next) {
		placer = result.emplace_after(placer);
		resolution & info = *placer;
		info.address = endpoint::create(family_t(a->ai_family));
		std::memcpy(&(info.address->sock_addr()), a->ai_addr, a->ai_addrlen);
		info.protocol = protocols(a->ai_protocol);
		info.socket_type = type;
	}
	return result;
}

std::forward_list<resolution> resolve(const std::string & host, const std::string & port, protocols protocol) {
	socket_types type;
	switch (protocol) {
	case protocols::tcp:
		type = socket_types::stream;
		break;
	case protocols::udp:
		type = socket_types::datagram;
		break;
	default:
		throw resolver_error("unknown protocol number");
	}
	return resolve(host, port, type, protocol);
}

std::forward_list<resolution> resolve(const std::string & host, const std::string & port, socket_types type) {
	protocols protocol;
	switch (type) {
	case socket_types::stream:
		protocol = protocols::tcp;
		break;
	case socket_types::datagram:
		protocol = protocols::udp;
		break;
	default:
		throw resolver_error("unknown socket type");
	}
	return resolve(host, port, type, protocol);
}

std::forward_list<resolution> resolve(socket_types type, const uri & address, std::uint16_t default_port) {
	const auto & scheme = address.get_scheme();
	auto port = address.get_port();
	if (port == -1)
		port = default_port;
	if (scheme == "udp") {
		if (type == socket_types::stream)
			throw resolver_error("udp is not a stream protocol");
		return resolve(address.get_host(), std::to_string(port), socket_types::datagram, protocols::udp);
	} else if (scheme == "tcp") {
		if (type == socket_types::datagram)
			throw resolver_error("tcp is not a datagram protocol");
		return resolve(address.get_host(), std::to_string(port), socket_types::stream, protocols::tcp);
	} else if (scheme == "unix") {
		if (type == socket_types::unknown)
			throw resolver_error("unable to deduce unix socket type");
		std::forward_list<resolution> result;
		resolution & it = result.emplace_front();
		it.socket_type = type;
		it.protocol = protocols::unknown;
		it.address = std::make_unique<un::endpoint>(address.get_path());
		return result;
	} else {
		throw resolver_error("unknown scheme name: " + scheme);
	}
}

std::unique_ptr<stream_socket_d> connect(const resolution & target, std::uint32_t flags) {
	if (target.socket_type != socket_types::stream)
		throw std::runtime_error("able to connect only stream socket");
	switch (target.address->family()) {
		case family_t::ipv4: {
			if (target.protocol != protocols::tcp)
				throw std::runtime_error("only tcp as ipv4 stream protocol supported");
			std::unique_ptr result = std::make_unique<client_tcp_socket_d>();
			result->connect(dynamic_cast<const ipv4::endpoint &>(*target.address), flags);
			return result;
		}
		case family_t::ipv6: {
			if (target.protocol != protocols::tcp)
				throw std::runtime_error("only tcp as ipv6 stream protocol supported");
			std::unique_ptr result = std::make_unique<client_tcp_socket_d>();
			result->connect(dynamic_cast<const ipv6::endpoint &>(*target.address), flags);
			return result;
		}
		case family_t::un: {
			std::unique_ptr result = std::make_unique<client_stream_unix_socket_d>();
			result->connect(dynamic_cast<const un::endpoint &>(*target.address), flags);
			return result;
		}
		default: throw std::runtime_error("unknown network family");
	}
}

std::unique_ptr<datagram_socket_d> prepare(const resolution & target, std::uint32_t flags) {
	if (target.socket_type != socket_types::datagram)
		throw std::runtime_error("able to prepare only datagram socket");
	family_t family = target.address->family();
	switch (family) {
		case family_t::ipv4:
		case family_t::ipv6: {
			if (target.protocol != protocols::udp)
				throw std::runtime_error("only udp as ipv4 datagram protocol supported");
			std::unique_ptr result = std::make_unique<client_udp_socket_d>();
			result->open(family, flags);
			return result;
		}
		case family_t::un: {
			std::unique_ptr result = std::make_unique<client_datagram_unix_socket_t>();
			result->open(flags);
			return result;
		}
		default: throw std::runtime_error("unknown network family");
	}
}

std::unique_ptr<stream_server_socket_d> bind_stream(const resolution & target, std::uint32_t flags) {
	if (target.socket_type != socket_types::stream)
		throw std::runtime_error("able to bind only stream socket");
	switch (target.address->family()) {
		case family_t::ipv4: {
			if (target.protocol != protocols::tcp)
				throw std::runtime_error("only tcp as ipv4 stream protocol supported");
			std::unique_ptr result = std::make_unique<server_tcp_socket_d>();
			result->bind(dynamic_cast<const ipv4::endpoint &>(*target.address), flags);
			return result;
		}
		case family_t::ipv6: {
			if (target.protocol != protocols::tcp)
				throw std::runtime_error("only tcp as ipv6 stream protocol supported");
			std::unique_ptr result = std::make_unique<server_tcp_socket_d>();
			result->bind(dynamic_cast<const ipv6::endpoint &>(*target.address), flags);
			return result;
		}
		case family_t::un: {
			std::unique_ptr result = std::make_unique<server_stream_unix_socket_d>();
			result->bind(dynamic_cast<const un::endpoint &>(*target.address), flags);
			return result;
		}
		default: throw std::runtime_error("unknown network family");
	}
}

std::unique_ptr<datagram_server_socket_d> bind_datagram(const resolution & target, std::uint32_t flags) {
	if (target.socket_type != socket_types::datagram)
		throw std::runtime_error("able to bind only datagram socket");
	switch (target.address->family()) {
		case family_t::ipv4: {
			if (target.protocol != protocols::udp)
				throw std::runtime_error("only udp as ipv4 datagram protocol supported");
			std::unique_ptr result = std::make_unique<server_udp_socket_d>();
			result->bind(dynamic_cast<const ipv4::endpoint &>(*target.address), flags);
			return result;
		}
		case family_t::ipv6: {
			if (target.protocol != protocols::udp)
				throw std::runtime_error("only udp as ipv6 datagram protocol supported");
			std::unique_ptr result = std::make_unique<server_udp_socket_d>();
			result->bind(dynamic_cast<const ipv6::endpoint &>(*target.address), flags);
			return result;
		}
		case family_t::un: {
			std::unique_ptr result = std::make_unique<server_datagram_unix_socket_d>();
			result->bind(dynamic_cast<const un::endpoint &>(*target.address), flags);
			return result;
		}
		default: throw std::runtime_error("unknown network family");
	}
}

std::unique_ptr<bound_socket_d> bind(const resolution & target, std::uint32_t flags) {
	switch (target.socket_type) {
		case socket_types::stream:
			return bind_stream(target, flags);
		case socket_types::datagram:
			return bind_datagram(target, flags);
		default:
			throw std::runtime_error("unknown socket type");
	}
}

} // namespace ekutils::net
