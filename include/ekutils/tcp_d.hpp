#ifndef TCP_D_HEAD_NFH14JGKBXGFC
#define TCP_D_HEAD_NFH14JGKBXGFC

#include <ekutils/listener_socket_d.hpp>

namespace ekutils {

class tcp_listener_d;
class tcp_socket_d;

class tcp_socket_d : public stream_socket_d {
	tcp_socket_d(int fd, const endpoint_info & local, const endpoint_info & remote, sock_flags::flags f);
public:
	tcp_socket_d(tcp_socket_d && other) : stream_socket_d(std::move(other)) {}
	tcp_socket_d() {}
	tcp_socket_d(const std::vector<connection_info> & infos, sock_flags::flags f = sock_flags::flags::nothing) : tcp_socket_d() {
		open(infos, f);
	}
	void open(const std::vector<connection_info> & infos, sock_flags::flags f = sock_flags::flags::nothing);
	virtual std::string to_string() const noexcept override;
	friend class tcp_listener_d;
};

class tcp_listener_d : public listener_socket_d {
private:
	sock_flags::flags flags;
	endpoint_info local_info;
public:
	explicit tcp_listener_d(sock_flags::flags f = sock_flags::nothing);
	tcp_listener_d(const std::string & address, const std::string & port,
		sock_flags::flags f = sock_flags::nothing);
	void listen(const std::string & address, const std::string & port,
		sock_flags::flags f = sock_flags::nothing);
	void listen(const std::string & address, std::uint16_t port,
		sock_flags::flags f = sock_flags::nothing) {
		listen(address, std::to_string(port), f);
	}
	virtual std::string to_string() const noexcept override;
	const endpoint_info & local_endpoint() const noexcept {
		return local_info;
	}
	tcp_socket_d accept();
	virtual std::unique_ptr<stream_socket_d> accept_virtual() override {
		return std::make_unique<tcp_socket_d>(accept());
	}
};

} // namespace ekutils

#endif // TCP_D_HEAD_NFH14JGKBXGFC
