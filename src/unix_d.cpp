#include "ekutils/unix_d.hpp"

#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <sys/socket.h>

#include "sys_error.hpp"

namespace ekutils::net {

void datagram_unix_socket_d::open(std::int32_t flags) {
	close();
	handle = socket(AF_UNIX, SOCK_DGRAM | ((flags & socket_flags::non_block) ? SOCK_NONBLOCK : 0), 0);
	if (handle == -1)
		sys_error("failed to create unix datagram socket");
}

std::string client_datagram_unix_socket_t::to_string() const noexcept {
	return "unix datagram client";
}

void server_datagram_unix_socket_d::bind(const un::endpoint & address, std::int32_t flags) {
	open(flags);
	info = address;
	if (flags & socket_flags::reuse_port) {
		int opt = 1;
		setsockopt(handle, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
	}
	if (::bind(handle, &address.sock_addr(), address.sock_len()) == -1)
		sys_error("failed to bind a unix socket");
}

const endpoint & server_datagram_unix_socket_d::local_endpoint() const {
	check_created();
	return info;
}

std::string server_datagram_unix_socket_d::to_string() const noexcept {
	if (*this) {
		return "unix datagram server (" + local_endpoint().to_string() + ')';
	} else {
		return "unix datagram server";
	}
}

void stream_unix_socket_d::open(std::int32_t flags) {
	close();
	handle = socket(AF_UNIX, SOCK_STREAM | ((flags & socket_flags::non_block) ? SOCK_NONBLOCK : 0), 0);
	if (handle == -1)
		sys_error("failed to create unix stream socket");
}

const endpoint & stream_unix_socket_d::local_endpoint() const {
	check_created();
	return info;
}

void client_stream_unix_socket_d::connect(const un::endpoint & address, std::uint32_t flags) {
	open(flags);
	info = address;
	if (::connect(handle, &address.sock_addr(), address.sock_len()) == -1 && errno != EINPROGRESS)
		sys_error("failed to connect to " + address.to_string());
}

const endpoint & client_stream_unix_socket_d::local_endpoint() const {
	check_created();
	return info;
}

const endpoint & client_stream_unix_socket_d::remote_endpoint() const {
	check_created();
	return info;
}

std::string client_stream_unix_socket_d::to_string() const noexcept {
	if (*this)
		return "unix stream client (" + info.to_string() + ')';
	else
		return "unix stream client";
}

void server_stream_unix_socket_d::bind(const un::endpoint & address, std::int32_t flags) {
	open(flags);
	info = address;
	if (flags & socket_flags::reuse_port) {
		int opt = 1;
		setsockopt(handle, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
	}
	if (::bind(handle, &address.sock_addr(), address.sock_len()) == -1)
		sys_error("failed to bind a unix socket");
}

const endpoint & server_stream_unix_socket_d::local_endpoint() const {
	check_created();
	return info;
}

client_stream_unix_socket_d server_stream_unix_socket_d::accept() {
	check_created();
	un::endpoint address;
	socklen_t socklen = address.sock_len();
	int client = ::accept(handle, &address.sock_addr(), &socklen);
	if (client == -1)
		sys_error("failed to accept the unix client");
	client_stream_unix_socket_d result(client);
	result.info = address;
	return result;
}

std::string server_stream_unix_socket_d::to_string() const noexcept {
	if (*this)
		return "unix stream server (" + info.to_string() + ')';
	else
		return "unix stream server";
}

} // namespace ekutils::net
