#ifndef STREAM_SOCKET_D_HEAD_QQPCPDKB37
#define STREAM_SOCKET_D_HEAD_QQPCPDKB37

#include <system_error>

#include <ekutils/connection_info.hpp>
#include <ekutils/io_stream_d.hpp>

namespace ekutils {

class stream_socket_d : public io_stream_d {
protected:
	sock_flags::flags flags;
	endpoint_info local_info;
	endpoint_info remote_info;
	
	stream_socket_d(int fd, const endpoint_info & local, const endpoint_info & remote, sock_flags::flags f);
public:
	stream_socket_d() :
			flags(sock_flags::flags::nothing),
			local_info(endpoint_info::empty), remote_info(endpoint_info::empty) {
		handle = -1;
	}

	stream_socket_d(stream_socket_d && other) :
			flags(other.flags),
			local_info(other.local_info),
			remote_info(other.remote_info) {
		descriptor::operator=(std::move(other));
	}

	std::errc ensure_connected();
	const endpoint_info & local_endpoint() const noexcept {
		return local_info;
	}
	const endpoint_info & remote_endpoint() const noexcept {
		return remote_info;
	}

	std::errc last_error();
	virtual int read(byte_t bytes[], size_t length) override;
	virtual int write(const byte_t bytes[], size_t length) override;

};

} // namespace ekutils

#endif // STREAM_SOCKET_D_HEAD_QQPCPDKB37
