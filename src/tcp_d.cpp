#include "ekutils/tcp_d.hpp"

#include <cstring>
#include <stdexcept>
#include <system_error>

#include <unistd.h>

#include "ekutils/putil.hpp"
#include "open_listener.hpp"

namespace ekutils {

tcp_socket_d::tcp_socket_d(int fd, const endpoint_info & local, const endpoint_info & remote,
		sock_flags::flags f) : stream_socket_d(fd, local, remote, f) {}

void tcp_socket_d::open(const std::vector<connection_info> & infos, sock_flags::flags f) {
	close();
	flags = f;
	for (const connection_info & info : infos) {
		handle = socket((int)info.endpoint.addr_family(), info.sock_type |
			((f & sock_flags::non_blocking) ? SOCK_NONBLOCK : 0), info.protocol);
		if (handle == -1)
			continue;
		if (connect(handle, &info.endpoint.info.addr, info.endpoint.addr_len()) != -1 || errno == EINPROGRESS) {
			remote_info = info.endpoint;
			return;
		}
		::close(handle);
	}
	throw std::runtime_error("unable to create socket");
}

std::string tcp_socket_d::to_string() const noexcept {
	return "tcp socket (" + std::string(local_info) + " <-> " + std::string(remote_info) + ")";
}

tcp_listener_d::tcp_listener_d(sock_flags::flags f) : flags(f), local_info(endpoint_info::empty) {
	handle = -1;
}

tcp_listener_d::tcp_listener_d(const std::string & address, const std::string & port,
		sock_flags::flags f) {
	handle = -1;
	listen(address, port, f);
}

void tcp_listener_d::listen(const std::string & address, const std::string & port, sock_flags::flags f) {
	close();
	flags = f;
	handle = open_listener(address, port, local_info, SOCK_STREAM, f);
}

void tcp_listener_d::start(int backlog) {
	if (::listen(handle, backlog) < 0)
		throw std::system_error(std::make_error_code(std::errc(errno)), "failed to start tcp listener");
	socklen_t len = local_info.addr_len();
	if (getsockname(handle, &local_info.info.addr, &len) == -1)
		throw std::system_error(std::make_error_code(std::errc(errno)), "failed to gain info about tcp socket");
}

std::string tcp_listener_d::to_string() const noexcept {
	return "tcp listener (" + std::string(local_info) + ')';
}

tcp_socket_d tcp_listener_d::accept() {
	static const socklen_t sockaddr_len = sizeof(sockaddr_in6);
	byte_t sockaddr_data[sockaddr_len];
	sockaddr * sockaddr_info = reinterpret_cast<sockaddr *>(sockaddr_data);
	socklen_t actual_len = sockaddr_len;
	int client = ::accept(handle, sockaddr_info, &actual_len);
	if (client < 0)
		throw std::runtime_error(std::string("failed to accept the tcp socket: ") + std::strerror(errno));
	endpoint_info socket_endpoint;
	sockaddr2endpoint(sockaddr_info, socket_endpoint);
	return tcp_socket_d(client, local_info, socket_endpoint, flags);
}

void tcp_listener_d::set_reusable(bool reuse) {
	int opt = reuse ? 1 : 0;
	if (setsockopt(handle, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1) {
		throw std::system_error(std::make_error_code(std::errc(errno)), "can't set SO_REUSEPORT option for tcp listener");
	}
}

} // namespace ektils
