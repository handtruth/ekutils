#ifndef UNIX_D_HEAD_YDBQTU6FTRS
#define UNIX_D_HEAD_YDBQTU6FTRS

#include <filesystem>

#include <ekutils/stream_socket_d.hpp>

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

class unix_stream_listener_d : public descriptor {
private:
	sock_flags::flags flags;
	endpoint_info local_info;
public:
	explicit unix_stream_listener_d(sock_flags::flags f = sock_flags::nothing);
	unix_stream_listener_d(const std::filesystem::path & path,
		sock_flags::flags f = sock_flags::nothing);
	void listen(const std::filesystem::path & path, sock_flags::flags f = sock_flags::nothing);
	void start(int backlog = 25);
	virtual std::string to_string() const noexcept override;
	const endpoint_info & local_endpoint() const noexcept {
		return local_info;
	}
	unix_stream_socket_d accept();
	void set_reusable(bool reuse = true);
};

} // namespace ekutils

#endif // UNIX_D_HEAD_YDBQTU6FTRS
