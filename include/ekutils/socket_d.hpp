#ifndef SOCKET_D_HEAD_UJTGRFRCGTHJU
#define SOCKET_D_HEAD_UJTGRFRCGTHJU

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <cinttypes>
#include <string>
#include <string_view>
#include <array>
#include <memory>
#include <system_error>

#include <ekutils/descriptor.hpp>
#include <ekutils/primitives.hpp>
#include <ekutils/io_stream_d.hpp>

namespace ekutils::net {

enum class family_t {
	unknown = AF_UNSPEC,
	ipv4 = AF_INET,
	ipv6 = AF_INET6,
	un = AF_UNIX,
};

struct endpoint {
	virtual family_t family() const noexcept = 0;
	virtual sockaddr & sock_addr() = 0;
	virtual const sockaddr & sock_addr() const = 0;
	virtual socklen_t sock_len() const noexcept = 0;
	virtual std::string to_string() const = 0;
	virtual ~endpoint() {}
	static std::unique_ptr<endpoint> create(family_t family);
};

typedef in_port_t port_t;

namespace ipv4 {

struct address final {
	in_addr_t data;
	address() = default;
	constexpr address(in_addr_t a) : data(a) {}
	address(const std::string_view & str);
	std::string to_string() const;
	constexpr operator in_addr_t() const noexcept {
		return data;
	}
};

static constexpr address loopback = INADDR_LOOPBACK;
static constexpr address any = INADDR_ANY;
static constexpr address broadcast = INADDR_BROADCAST;

class endpoint final : public net::endpoint {
	sockaddr_in addr;
public:
	endpoint() {
		addr.sin_family = AF_INET;
	}
	endpoint(const ipv4::address & ipaddr, port_t port);

	virtual family_t family() const noexcept override {
		return family_t::ipv4;
	}
	virtual const sockaddr & sock_addr() const override {
		return reinterpret_cast<const sockaddr &>(addr);
	}
	virtual sockaddr & sock_addr() override {
		return reinterpret_cast<sockaddr &>(addr);
	}
	virtual socklen_t sock_len() const noexcept override {
		return sizeof(addr);
	}
	ipv4::address address() const {
		return addr.sin_addr.s_addr;
	}
	port_t port() const noexcept;
	virtual std::string to_string() const override;
};

} // namespace ipv4

namespace ipv6 {

struct address final {
	std::array<std::uint8_t, 16> data;
	address() = default;
	address(const in6_addr & a);
	address(const std::string_view & str);
	std::string to_string() const;
	operator in6_addr() const noexcept;
};

inline const address loopback = in6addr_loopback;
inline const address any = in6addr_any;

class endpoint final : public net::endpoint {
	sockaddr_in6 addr;
public:
	endpoint() {
		addr.sin6_family = AF_INET6;
	}
	endpoint(const address & ipaddr, port_t port);

	virtual family_t family() const noexcept override {
		return family_t::ipv6;
	}
	virtual const sockaddr & sock_addr() const override {
		return reinterpret_cast<const sockaddr &>(addr);
	}
	virtual sockaddr & sock_addr() override {
		return reinterpret_cast<sockaddr &>(addr);
	}
	virtual socklen_t sock_len() const noexcept override {
		return sizeof(addr);
	}
	ipv6::address address() const noexcept {
		return addr.sin6_addr;
	}
	port_t port() const noexcept;
	virtual std::string to_string() const override;
};

} // namespace ipv6

namespace un {

class endpoint final : public net::endpoint {
	sockaddr_un addr;
public:
	endpoint() {
		addr.sun_family = AF_UNIX;
	}
	explicit endpoint(const std::string_view & path, bool is_abstract = false);

	virtual family_t family() const noexcept override {
		return family_t::un;
	}
	virtual const sockaddr & sock_addr() const override {
		return reinterpret_cast<const sockaddr &>(addr);
	}
	virtual sockaddr & sock_addr() override {
		return reinterpret_cast<sockaddr &>(addr);
	}
	virtual socklen_t sock_len() const noexcept override {
		return sizeof(addr);
	}
	bool is_abstract() const noexcept {
		return !addr.sun_path[0];
	}
	const std::string_view address() const noexcept {
		if (is_abstract())
			return addr.sun_path + 1;
		else
			return addr.sun_path;
	}
	virtual std::string to_string() const override;
};

} // namespace un

enum class socket_types {
	unknown = 0,
	stream = SOCK_STREAM,
	datagram = SOCK_DGRAM,
	/* not interested in others right now */
};

enum class protocols {
	unknown = IPPROTO_IP,
	tcp = IPPROTO_TCP,
	udp = IPPROTO_UDP,
	/* not interested in others right now */
};

struct socket_d : public virtual descriptor {
	socket_d & operator=(socket_d && other) = delete;
	std::errc last_error() const;
	virtual family_t family() const noexcept = 0;
	virtual socket_types type() const noexcept = 0;
};

struct ip_socket_d : public virtual socket_d {
	virtual protocols protocol() const noexcept = 0;
};

struct bound_socket_d : public virtual socket_d {
	virtual const endpoint & local_endpoint() const = 0;
};

struct stream_socket_d : public virtual bound_socket_d, public virtual io_stream_d {
	virtual socket_types type() const noexcept final {
		return socket_types::stream;
	}
	virtual const endpoint & remote_endpoint() const = 0;
	virtual int read(byte_t bytes[], size_t length) override;
	virtual int write(const byte_t bytes[], size_t length) override;
};

struct stream_server_socket_d : public virtual bound_socket_d {
	virtual socket_types type() const noexcept final {
		return socket_types::stream;
	}
	void listen(int backlog = 10);
	virtual std::unique_ptr<stream_socket_d> accept_virtual() = 0;
};

struct datagram_socket_d : public virtual socket_d {
	virtual socket_types type() const noexcept final {
		return socket_types::datagram;
	}

	int read(byte_t bytes[], std::size_t length);
	int read(byte_t bytes[], std::size_t length, std::unique_ptr<endpoint> & remote);
	int write(const byte_t bytes[], size_t length, const endpoint & remote);
};

struct datagram_server_socket_d : public virtual datagram_socket_d, public virtual bound_socket_d {};

namespace socket_flags {
	enum flags : std::int32_t {
		nothing = 0, reuse_port = 1, non_block = 2
	};
}

} // namespace ekutils::net

#endif // SOCKET_D_HEAD_UJTGRFRCGTHJU
