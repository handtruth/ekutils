#ifndef UNIX_D_HEAD_YDBQTU6FTRS
#define UNIX_D_HEAD_YDBQTU6FTRS

#include <filesystem>

#include <ekutils/listener_socket_d.hpp>

namespace ekutils {

class unix_stream_listener_d;
class unix_stream_socket_d;

class unix_stream_socket_d : public stream_socket_d {
	unix_stream_socket_d(int fd, const endpoint_info & local, const endpoint_info & remote, sock_flags::flags f);
public:
	unix_stream_socket_d(unix_stream_socket_d && other) : stream_socket_d(std::move(other)) {};
	unix_stream_socket_d() {}
	unix_stream_socket_d(const std::filesystem::path & path, sock_flags::flags f = sock_flags::flags::nothing) : unix_stream_socket_d() {
		open(path, f);
	}
	void open(const std::filesystem::path & path, sock_flags::flags f = sock_flags::flags::nothing);
	virtual std::string to_string() const noexcept override;
	friend class unix_stream_listener_d;
};

class unix_stream_listener_d : public listener_socket_d {
private:
	sock_flags::flags flags;
	endpoint_info local_info;
public:
	explicit unix_stream_listener_d(sock_flags::flags f = sock_flags::nothing);
	unix_stream_listener_d(const std::filesystem::path & path,
		sock_flags::flags f = sock_flags::nothing);
	void listen(const std::filesystem::path & path, sock_flags::flags f = sock_flags::nothing);
	virtual std::string to_string() const noexcept override;
	const endpoint_info & local_endpoint() const noexcept {
		return local_info;
	}
	unix_stream_socket_d accept();
	virtual std::unique_ptr<stream_socket_d> accept_virtual() override {
		return std::make_unique<unix_stream_socket_d>(accept());
	}
};

} // namespace ekutils

#endif // UNIX_D_HEAD_YDBQTU6FTRS
