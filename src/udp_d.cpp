#include "ekutils/udp_d.hpp"

#include <cstring>
#include <stdexcept>
#include <cerrno>

#include "sys_error.hpp"
#include "net_utils.hpp"

namespace ekutils::net {

void udp_socket_d::open(family_t family, std::int32_t flags) {
	close();
	m_family = family;
	handle = socket(int(family), SOCK_DGRAM | ((flags & socket_flags::non_block) ? SOCK_NONBLOCK : 0), IPPROTO_UDP);
	if (handle == -1)
		sys_error("failed to create udp socket");
}

std::string client_udp_socket_d::to_string() const noexcept {
	return "udp client";
}

void server_udp_socket_d::bind_private(const endpoint & address, std::int32_t flags) {
	if (flags & socket_flags::reuse_port) {
		int opt = 1;
		setsockopt(handle, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
	}
	if (::bind(handle, &address.sock_addr(), address.sock_len()) == -1)
		sys_error("failed to bind a udp socket");
}

void server_udp_socket_d::bind(const ipv4::endpoint & address, std::int32_t flags) {
	open(family_t::ipv4, flags);
	local_info = address;
	bind_private(address, flags);
}

void server_udp_socket_d::bind(const ipv6::endpoint & address, std::int32_t flags) {
	open(family_t::ipv6, flags);
	local_info = address;
	bind_private(address, flags);
}

const endpoint & server_udp_socket_d::local_endpoint() const {
	check_created();
	return *get_endpoint(local_info);
}

std::string server_udp_socket_d::to_string() const noexcept {
	if (*this)
		return "udp server (" + get_endpoint(local_info)->to_string() + ')';
	else
		return "udp server";
}

} // namespace ekutils::net
