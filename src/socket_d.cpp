#include "ekutils/socket_d.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <cassert>

#include "sys_error.hpp"

namespace ekutils::net {

std::unique_ptr<endpoint> endpoint::create(family_t family) {
	switch (int(family)) {
		case AF_INET:
			return std::make_unique<ipv4::endpoint>();
		case AF_INET6:
			return std::make_unique<ipv6::endpoint>();
		case AF_UNIX:
			return std::make_unique<ipv6::endpoint>();
		default:
			return nullptr;
	}
}

namespace ipv4 {

address::address(const std::string_view & str) {
	constexpr std::size_t length = INET_ADDRSTRLEN;
	if (str.size() != length - 1)
		throw std::runtime_error("wrong ipv4 address size");
	char buffer[length];
	std::memcpy(buffer, str.data(), length);
	buffer[length - 1] = '\0';
	inet_pton(AF_INET, buffer, &data);
}

std::string address::to_string() const {
	constexpr socklen_t length = INET_ADDRSTRLEN;
	char buffer[length];
	inet_ntop(AF_INET, &data, buffer, length);
	return buffer;
}

endpoint::endpoint(const ipv4::address & ipaddr, port_t port) : addr { /* zero */ } {
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ipaddr.data;
	addr.sin_port = htons(port);
}

port_t endpoint::port() const noexcept {
	return ntohs(addr.sin_port);
}

std::string endpoint::to_string() const {
	return address().to_string() + ':' + std::to_string(port());
}

} // namespace ipv4

namespace ipv6 {

address::address(const in6_addr & other) {
	std::memcpy(data.data(), other.s6_addr, data.size());
}

address::address(const std::string_view & str) {
	constexpr std::size_t length = INET6_ADDRSTRLEN;
	if (str.size() > length - 1)
		throw std::runtime_error("wrong ipv6 address size");
	char buffer[length];
	std::memcpy(buffer, str.data(), str.size());
	buffer[str.size()] = '\0';
	inet_pton(AF_INET6, buffer, data.data());
}

std::string address::to_string() const {
	constexpr socklen_t length = INET6_ADDRSTRLEN;
	char buffer[length];
	inet_ntop(AF_INET6, data.data(), buffer, length);
	return buffer;
}

address::operator in6_addr() const noexcept {
	in6_addr other;
	std::memcpy(other.s6_addr, data.data(), data.size());
	return other;
}

endpoint::endpoint(const ipv6::address & ipaddr, port_t port) : addr { /* zero */ } {
	addr.sin6_family = AF_INET6;
	addr.sin6_addr = ipaddr;
	addr.sin6_port = htons(port);
}

port_t endpoint::port() const noexcept {
	return ntohs(addr.sin6_port);
}

std::string endpoint::to_string() const {
	return '[' + address().to_string() + "]:" + std::to_string(port());
}

} // namespace ipv6

namespace un {

endpoint::endpoint(const std::string_view & path, bool is_abstract) : addr { /* zero */ } {
	constexpr std::size_t max_size = sizeof(addr.sun_path) - 1;
	if (path.size() > max_size - is_abstract)
		throw std::runtime_error("address is too big for unix socket");
	addr.sun_family = AF_UNIX;
	addr.sun_path[0] = '\0';
	std::memcpy(addr.sun_path + is_abstract, path.data(), path.size());
}

std::string endpoint::to_string() const {
	const std::string_view & a = address();
	if (is_abstract()) {
		std::string result = "abstract: ";
		result += address();
		return result;
	} else {
		return std::string(a);
	}
}

} // namespace un

std::errc socket_d::last_error() const {
	check_created();
	int err;
	socklen_t sz = sizeof(err);
	if (getsockopt(handle, SOL_SOCKET, SO_ERROR, &err, &sz) == -1)
		sys_error("failed to get socket last error");
	return std::errc(err);
}

int datagram_socket_d::read(byte_t bytes[], std::size_t length) {
	check_created();
	int r = recvfrom(handle, bytes, length, 0, nullptr, nullptr);
	if (r == -1)
		sys_error("failed to read from udp socket");
	return r;
}

int datagram_socket_d::read(byte_t bytes[], std::size_t length, std::unique_ptr<endpoint> & remote) {
	check_created();
	remote = endpoint::create(family());
	socklen_t size = remote->sock_len();
	socklen_t socklen = size;
	int r = recvfrom(handle, bytes, length, 0, &remote->sock_addr(), &socklen);
	assert(size == socklen);
	if (r == -1)
		sys_error("failed to read from udp socket");
	return r;
}

int datagram_socket_d::write(const byte_t bytes[], size_t length, const endpoint & remote) {
	check_created();
	int r = sendto(handle, bytes, length, 0, &remote.sock_addr(), remote.sock_len());
	if (r < 0)
		sys_error("failed to send by udp socket");
	return r;
}

int stream_socket_d::read(byte_t bytes[], size_t length) {
	int r=recv(handle, bytes, length, 0);
	if (r < 0 && errno != EWOULDBLOCK)
		sys_error("failed to read from socket \"" + local_endpoint().to_string() + "\"");
	return r;
}

int stream_socket_d::write(const byte_t bytes[], size_t length) {
	int r = ::send(handle, bytes, length, 0);
	if (r < 0 && errno != EWOULDBLOCK)
		sys_error("failed to write to socket \"" + local_endpoint().to_string() + "\"");
	return r;
}

void stream_server_socket_d::listen(int backlog) {
	check_created();
	if (::listen(handle, backlog) == -1)
		sys_error("failed to start " + to_string());
}

} // namespace ekutils::net
