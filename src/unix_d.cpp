#include "ekutils/unix_d.hpp"

#include <cstring>
#include <stdexcept>

#include <unistd.h>
#include <sys/socket.h>

#include "open_listener.hpp"
#include "ekutils/putil.hpp"

namespace ekutils {

void setup_un_addrinfo(endpoint_info & info, const std::filesystem::path & path) {
	sockaddr_un & addr = info.info.addr_un;
	memset(&addr, 0, sizeof(sockaddr_un));
	addr.sun_family = AF_UNIX;
	if (std::strlen(path.c_str()) >= sizeof(addr.sun_path))
		throw std::runtime_error("path is too big for unix socket: \"" + path.string() + '"');
	strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path) - 1);
}

unix_stream_socket_d::unix_stream_socket_d(int fd, const endpoint_info & local, const endpoint_info & remote,
		sock_flags::flags f) : stream_socket_d(fd, local, remote, f) {}

void unix_stream_socket_d::open(const std::filesystem::path & path, sock_flags::flags f) {
	close();
	flags = f;
	handle = socket(AF_UNIX, SOCK_STREAM | ((f & sock_flags::non_blocking) ? SOCK_NONBLOCK : 0), 0);
	if (handle == -1) {
		throw std::system_error(std::make_error_code(std::errc(errno)),
			"failed create unix socket \"" + path.string() + "\"");
	}
	setup_un_addrinfo(remote_info, path);
	if (connect(handle, &remote_info.info.addr, remote_info.addr_len()) != -1 || errno == EINPROGRESS) {
		return;
	}
	auto code = errno;
	::close(handle);
	throw std::system_error(std::make_error_code(std::errc(code)), "failed to connect to unix socket: \"" + path.string() + '"');
}

std::string unix_stream_socket_d::to_string() const noexcept {
	return "unix socket (" + std::string(remote_info) + ")";
}

unix_stream_listener_d::unix_stream_listener_d(sock_flags::flags f) : flags(f), local_info(endpoint_info::empty) {
	handle = -1;
}

unix_stream_listener_d::unix_stream_listener_d(const std::filesystem::path & path, sock_flags::flags f) {
	handle = -1;
	listen(path, f);
}

void unix_stream_listener_d::listen(const std::filesystem::path & path, sock_flags::flags f) {
	close();
	flags = f;
	handle = socket(AF_UNIX, SOCK_STREAM, 0);
	if (handle == -1)
		throw std::system_error(std::make_error_code(std::errc(errno)), "failed to create unix listener socket");
	setup_un_addrinfo(local_info, path);
	int status = bind(handle, &local_info.info.addr, local_info.addr_len());
	if (status == -1)
		throw std::system_error(std::make_error_code(std::errc(errno)), "failed to bind unix socket: \"" + path.string() + '"');
}

std::string unix_stream_listener_d::to_string() const noexcept {
	return "unix listener (" + std::string(local_info) + ')';
}

unix_stream_socket_d unix_stream_listener_d::accept() {
	static const socklen_t sockaddr_len = sizeof(sockaddr_in6);
	byte_t sockaddr_data[sockaddr_len];
	sockaddr * sockaddr_info = reinterpret_cast<sockaddr *>(sockaddr_data);
	socklen_t actual_len = sockaddr_len;
	int client = ::accept(handle, sockaddr_info, &actual_len);
	if (client < 0)
		throw std::runtime_error(std::string("failed to accept the unix socket: ") + std::strerror(errno));
	endpoint_info socket_endpoint;
	sockaddr2endpoint(sockaddr_info, socket_endpoint);
	return unix_stream_socket_d(client, local_info, socket_endpoint, flags);
}

} // namespace ekutils

