#ifndef _SOCKET_D_HEAD
#define _SOCKET_D_HEAD

#include <vector>
#include <system_error>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <ekutils/primitives.hpp>
#include <ekutils/in_stream_d.hpp>
#include <ekutils/out_stream_d.hpp>

namespace ekutils {

class tcp_listener_d;
class udp_server_d;
class tcp_socket_d;

struct endpoint_info {
	static endpoint_info empty;

	enum class family_t {
		ipv4 = AF_INET,
		ipv6 = AF_INET6,
	};

	union {
		sockaddr addr;
		sockaddr_in addr_in;
		sockaddr_in6 addr_in6;
	} info;

	socklen_t addr_len() const {
		switch (addr_family()) {
			case family_t::ipv4: return sizeof(sockaddr_in);
			case family_t::ipv6: return sizeof(sockaddr_in6);
			default: return -1;
		}
	}

	family_t addr_family() const {
		return (family_t)info.addr.sa_family;
	}
	std::uint16_t port() const {
		uint16_t p;
		switch (addr_family()) {
			case family_t::ipv4:
				p = info.addr_in.sin_port;
				break;
			case family_t::ipv6:
				p = info.addr_in6.sin6_port;
				break;
			default:
				return 0;
		}
		return p >> 8 | p << 8;
	}
	std::string address() const;
	operator std::string() const {
		return address() + ':' + std::to_string(port());
	}
};

class dns_error : public std::runtime_error {
public:
	explicit dns_error(const std::string & message) : std::runtime_error(message) {}
	explicit dns_error(const char * message) : std::runtime_error(message) {}
};

struct connection_info {
	endpoint_info endpoint;
	int sock_type;
	int protocol;
	static std::vector<connection_info> resolve(const std::string & address, const std::string & port);
	static std::vector<connection_info> resolve(const std::string & address, std::uint16_t port) {
		return resolve(address, std::to_string(port));
	}
};

namespace tcp_flags {
	enum flags : std::int32_t {
		nothing, non_blocking, reuse_port
	};
}

class tcp_socket_d : public in_stream_d, public out_stream_d {
private:
	tcp_flags::flags flags;
	endpoint_info local_info;
	endpoint_info remote_info;
	tcp_socket_d(int fd, const endpoint_info & local, const endpoint_info & remote, tcp_flags::flags f);
public:
	tcp_socket_d(tcp_socket_d && other) :
			flags(other.flags),
			local_info(other.local_info),
			remote_info(other.remote_info) {
		handle = other.handle;
		other.handle = -1;
	}
	tcp_socket_d() :
			flags(tcp_flags::flags::nothing),
			local_info(endpoint_info::empty), remote_info(endpoint_info::empty) {
		handle = -1;
	}
	void open(const std::vector<connection_info> & infos, tcp_flags::flags f = tcp_flags::flags::nothing);
	std::errc ensure_connected();
	virtual std::string to_string() const noexcept override;
	const endpoint_info & local_endpoint() const noexcept {
		return local_info;
	}
	const endpoint_info & remote_endpoint() const noexcept {
		return remote_info;
	}
	std::errc last_error();
	virtual int read(byte_t bytes[], size_t length) override;
	virtual int write(const byte_t bytes[], size_t length) override;
	virtual ~tcp_socket_d() override;
	friend class tcp_listener_d;
};

class tcp_listener_d : public descriptor {
private:
	tcp_flags::flags flags;
	endpoint_info local_info;
public:
	explicit tcp_listener_d(tcp_flags::flags f = tcp_flags::nothing);
	tcp_listener_d(const std::string & address, std::uint16_t port,
		tcp_flags::flags f = tcp_flags::nothing);
	void listen(const std::string & address, std::uint16_t port,
		tcp_flags::flags f = tcp_flags::nothing);
	void start(int backlog = 25);
	virtual std::string to_string() const noexcept override;
	const endpoint_info & local_endpoint() const noexcept {
		return local_info;
	}
	tcp_socket_d accept();
	void set_reusable(bool reuse = true);
	virtual ~tcp_listener_d() override;
};

class udp_server_d : public descriptor {
private:
	tcp_flags::flags flags;
	endpoint_info local_info;
public:
	udp_server_d(const std::string & address, std::uint16_t port, tcp_flags::flags f = tcp_flags::nothing);
	virtual std::string to_string() const noexcept override;
	int read(byte_t bytes[], size_t length, endpoint_info * remote_endpoint);
	int write(const byte_t bytes[], size_t length, const endpoint_info & remote_endpoint);
	const endpoint_info & local_endpoint() const noexcept {
		return local_info;
	}
	virtual ~udp_server_d() override;
};

} // namespace ekutils


#endif // _SOCKET_D_HEAD
