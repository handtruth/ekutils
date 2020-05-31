#include "ekutils/socket_d.hpp"

#include <cstring>
#include <stdexcept>

#include "ekutils/putil.hpp"
#include "open_listener.hpp"

namespace ekutils {

udp_server_d::udp_server_d(const std::string & address, const std::string & port, sock_flags::flags f) {
	handle = open_listener(address, port, local_info, SOCK_DGRAM, f);
}

std::string udp_server_d::to_string() const noexcept {
	return "udp server (" + std::string(local_info) + ')';
}

int udp_server_d::read(byte_t bytes[], size_t length, endpoint_info * remote_endpoint) {
	static const socklen_t sockaddr_len = sizeof(sockaddr_in6);
	byte_t sockaddr_data[sockaddr_len];
	sockaddr * sockaddr_info = reinterpret_cast<sockaddr *>(sockaddr_data);
	socklen_t actual_len = sockaddr_len;
	int r = recvfrom(handle, bytes, length, 0, sockaddr_info, &actual_len);
	if (r < 0)
		throw std::system_error(std::make_error_code(std::errc(errno)),
			"failed to read from udp socket");
	if (remote_endpoint)
		sockaddr2endpoint(sockaddr_info, *remote_endpoint);
	return r;
}

int udp_server_d::write(const byte_t bytes[], size_t length, const endpoint_info & remote_endpoint) {
	int r = sendto(handle, bytes, length, 0, &(remote_endpoint.info.addr), sizeof(remote_endpoint.info));
	if (r < 0)
		throw std::system_error(std::make_error_code(std::errc(errno)),
			"failed to send by udp socket");
	return r;
}

udp_server_d::~udp_server_d() {}

} // namespace ekutils
