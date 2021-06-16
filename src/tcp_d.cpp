#include "ekutils/tcp_d.hpp"

#include <cstring>
#include <stdexcept>
#include <system_error>

#include "sys_error.hpp"
#include "net_utils.hpp"

namespace ekutils::net {

void tcp_socket_d::open(family_t family, std::int32_t flags) {
	close();
	m_family = family;
	handle = socket(int(family), SOCK_STREAM | ((flags & socket_flags::non_block) ? SOCK_NONBLOCK : 0), IPPROTO_TCP);
	if (handle == -1)
		sys_error("failed to create tcp socket");
}

void client_tcp_socket_d::connect_private(const endpoint & address) {
	if (::connect(handle, &address.sock_addr(), address.sock_len()) == -1 && errno != EINPROGRESS)
		sys_error("failed to connect to " + address.to_string());
}

void client_tcp_socket_d::connect(const ipv4::endpoint & address, std::uint32_t f) {
	open(family_t::ipv4, f);
	remote_info = address;
	connect_private(address);
}

void client_tcp_socket_d::connect(const ipv6::endpoint & address, std::uint32_t f) {
	open(family_t::ipv6, f);
	remote_info = address;
	connect_private(address);
}

const endpoint * client_tcp_socket_d::try_local_endpoint() const {
	check_created();
	if (std::holds_alternative<std::monostate>(local_info)) {
		initialize_endpoint(local_info, m_family);
		endpoint * it = get_endpoint(local_info);
		if (!local_endpoint_of(*it, *this)) {
			local_info = std::monostate();
			return nullptr;
		}
		return it;
	}
	return get_endpoint(local_info);
}

const endpoint & client_tcp_socket_d::local_endpoint() const {
	const endpoint * addr = try_local_endpoint();
	if (!addr)
		sys_error("failed to gain local endpoint");
	return *addr;
}

const endpoint & client_tcp_socket_d::remote_endpoint() const {
	check_created();
	return *get_endpoint(remote_info);
}

std::string client_tcp_socket_d::to_string() const noexcept {
	if (*this) {
		const endpoint * local = try_local_endpoint();
		return "tcp client (" + (local ? local->to_string() : std::string("?")) + " <-> " + remote_endpoint().to_string() + ")";
	} else {
		return "tcp client";
	}
}


void server_tcp_socket_d::bind_private(const endpoint & address, std::int32_t flags) {
	if (flags & socket_flags::reuse_port) {
		int opt = 1;
		setsockopt(handle, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
	}
	if (::bind(handle, &address.sock_addr(), address.sock_len()) == -1)
		sys_error("failed to bind a udp socket");
}

void server_tcp_socket_d::bind(const ipv4::endpoint & address, std::int32_t flags) {
	open(family_t::ipv4, flags);
	local_info = address;
	bind_private(address, flags);
}

void server_tcp_socket_d::bind(const ipv6::endpoint & address, std::int32_t flags) {
	open(family_t::ipv6, flags);
	local_info = address;
	bind_private(address, flags);
}

const endpoint & server_tcp_socket_d::local_endpoint() const {
	check_created();
	auto & endpoint = *get_endpoint(local_info);
	auto socklen = endpoint.sock_len();
	getsockname(handle, &endpoint.sock_addr(), &socklen);
	return *get_endpoint(local_info);
}

std::string server_tcp_socket_d::to_string() const noexcept {
	if (*this) {
		return "tcp server (" + local_endpoint().to_string() + ')';
	} else {
		return "tcp server";
	}
}

client_tcp_socket_d server_tcp_socket_d::accept() {
	check_created();
	net_variant address;
	initialize_endpoint(address, m_family);
	endpoint * ptr = get_endpoint(address);
	socklen_t socklen = ptr->sock_len();
	int client = ::accept(handle, &ptr->sock_addr(), &socklen);
	if (client == -1)
		sys_error("failed to accept the tcp client");
	client_tcp_socket_d result(client, m_family);
	result.remote_info = address;
	return result;
}

} // namespace ektils::net
