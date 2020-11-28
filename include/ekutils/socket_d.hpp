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
#include <chrono>

#include <ekutils/descriptor.hpp>
#include <ekutils/primitives.hpp>
#include <ekutils/io_stream_d.hpp>
#include <ekutils/beside.hpp>

namespace ekutils::net {

inline bool is_big_endian_f() {
    union {
        std::uint16_t i;
        char c[2];
    } bint = { 0x01000 };
    return bint.c[0];
}

static const bool is_big_endian = is_big_endian_f();

template <typename int_t>
int_t hton(int_t integer) {
	if (is_big_endian) {
		return integer;
	} else {
		constexpr std::size_t size = sizeof(int_t);
		union {
			int_t full;
			byte_t bytes[size];
		} number { integer };
		for (std::size_t i = 0; i < size/2; i++) {
			byte_t tmp = number.bytes[i];
			number.bytes[i] = number.bytes[size - i - 1];
			number.bytes[size - i - 1] = tmp;
		}
		return number.full;
	}
}

template <typename int_t>
int_t ntoh(int_t integer) {
	return hton(integer);
}

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
	bool operator==(const address & other) const noexcept {
		return data == other.data;
	}
	bool operator<(const address & other) const noexcept {
		return ntoh(data) < ntoh(other.data);
	}
	bool operator>(const address & other) const noexcept {
		return ntoh(data) > ntoh(other.data);
	}
	bool operator<=(const address & other) const noexcept {
		return ntoh(data) <= ntoh(other.data);
	}
	bool operator>=(const address & other) const noexcept {
		return ntoh(data) >= ntoh(other.data);
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
	bool operator==(const address & other) const noexcept {
		return data == other.data;
	}
	bool operator<(const address & other) const noexcept {
		return data < other.data;
	}
	bool operator>(const address & other) const noexcept {
		return data > other.data;
	}
	bool operator<=(const address & other) const noexcept {
		return data <= other.data;
	}
	bool operator>=(const address & other) const noexcept {
		return data >= other.data;
	}
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

	void recv_timeout(std::uint64_t seconds, std::uint64_t micros);
	void send_timeout(std::uint64_t seconds, std::uint64_t micros);

private:
	template <typename Rep, typename Period>
	void destruct_time(std::uint64_t & secs, std::uint64_t & micros, std::chrono::duration<Rep, Period> span) {
		using namespace std::chrono;
		seconds span_sec = duration_cast<seconds>(span);
		secs = static_cast<unsigned long>(span_sec.count());
		micros = static_cast<unsigned long>(duration_cast<microseconds>(span_sec - floor<seconds>(span_sec)).count());
	}

public:
	template <typename Rep, typename Period>
	void recv_timeout(std::chrono::duration<Rep, Period> span) {
		std::uint64_t secs, micros;
		destruct_time(secs, micros, span);
		recv_timeout(secs, micros);
	}

	template <typename Rep, typename Period>
	void send_timeout(std::chrono::duration<Rep, Period> span) {
		std::uint64_t secs, micros;
		destruct_time(secs, micros, span);
		send_timeout(secs, micros);
	}

	template <typename Rep, typename Period>
	void timeout(std::chrono::duration<Rep, Period> span) {
		std::uint64_t secs, micros;
		destruct_time(secs, micros, span);
		recv_timeout(secs, micros);
		send_timeout(secs, micros);
	}
	
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

namespace ekutils {

template<>
struct beside<net::ipv4::address> {
	bool operator()(const net::ipv4::address & a, const net::ipv4::address & b) const noexcept {
		return net::ntoh(a.data) + 1 == net::ntoh(b.data);
	}
};

template<>
struct beside<net::ipv6::address> {
	bool operator()(const net::ipv6::address & a, const net::ipv6::address & b) const noexcept;
};

} // namespace ekutils

namespace std {

template<>
struct hash<ekutils::net::ipv4::address> {
	std::size_t operator()(const ekutils::net::ipv4::address & address) const noexcept {
		return address.data;
	}
};

template<>
struct hash<ekutils::net::ipv6::address> {
	std::size_t operator()(const ekutils::net::ipv6::address & address) const noexcept {
		uint64_t most = *reinterpret_cast<const uint64_t *>(address.data.data());
		uint64_t least = *reinterpret_cast<const uint64_t *>(address.data.data() + 8);
		return most ^ least;
	}
};

inline string to_string(const ekutils::net::ipv4::address & address) {
	return address.to_string();
}

inline string to_string(const ekutils::net::ipv6::address & address) {
	return address.to_string();
}

} // namespace std

#endif // SOCKET_D_HEAD_UJTGRFRCGTHJU
