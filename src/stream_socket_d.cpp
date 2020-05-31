#include "ekutils/stream_socket_d.hpp"

#include <unistd.h>

namespace ekutils {

stream_socket_d::stream_socket_d(int fd, const endpoint_info & local, const endpoint_info & remote,
		sock_flags::flags f) :
	flags(f), local_info(local), remote_info(remote) {
	handle = fd;
}

std::errc stream_socket_d::ensure_connected() {
	socklen_t len = sizeof(struct sockaddr_in6);
	if (getpeername(handle, &local_info.info.addr, &len) == -1)
		return std::errc(errno);
	return std::errc(0);
}

std::errc stream_socket_d::last_error() {
	int err;
	socklen_t sz = sizeof(err);
	if (getsockopt(handle, SOL_SOCKET, SO_ERROR, &err, &sz) == -1)
		return std::errc(errno);
	return std::errc(err);
}

int stream_socket_d::read(byte_t bytes[], size_t length) {
	int r=recv(handle, bytes, length, 0);
	if (r < 0 && errno != EWOULDBLOCK)
		throw std::system_error(std::make_error_code(std::errc(errno)),
			"failed to read from socket \"" + std::string(remote_info) + "\"");
	return r;
}

int stream_socket_d::write(const byte_t bytes[], size_t length) {
	int r = ::write(handle, bytes, length);
	if (r < 0 && errno != EWOULDBLOCK)
		throw std::system_error(std::make_error_code(std::errc(errno)),
			"failed to write to socket \"" + std::string(remote_info) + "\"");
	return r;
}

} // namespace ekutils
