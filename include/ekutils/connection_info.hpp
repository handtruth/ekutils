#ifndef CONNECTION_INFO_HEAD_JKUHGUYXFJUK
#define CONNECTION_INFO_HEAD_JKUHGUYXFJUK

#include <stdexcept>
#include <string>
#include <vector>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>

#include <ekutils/primitives.hpp>

namespace ekutils {

struct endpoint_info {
	static endpoint_info empty;

	enum class family_t {
		ipv4 = AF_INET,
		ipv6 = AF_INET6,
		un = AF_UNIX,
	};

	union {
		sockaddr addr;
		sockaddr_in addr_in;
		sockaddr_in6 addr_in6;
		sockaddr_un addr_un;
	} info;

	socklen_t addr_len() const {
		switch (addr_family()) {
			case family_t::ipv4: return sizeof(sockaddr_in);
			case family_t::ipv6: return sizeof(sockaddr_in6);
			case family_t::un: return sizeof(sockaddr_un);
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
		if (info.addr.sa_family == AF_UNIX)
			return address();
		else
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

namespace sock_flags {
	enum flags : std::int32_t {
		nothing, non_blocking, reuse_port
	};
}

} // namespace ekutils

#endif // CONNECTION_INFO_HEAD_JKUHGUYXFJUK
