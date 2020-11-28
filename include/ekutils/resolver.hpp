#ifndef RESOLVER_HEAD_QPOVFVERFS
#define RESOLVER_HEAD_QPOVFVERFS

#include <forward_list>
#include <type_traits>
#include <functional>

#include <ekutils/socket_d.hpp>
#include <ekutils/uri.hpp>

namespace ekutils::net {

struct resolution {
	socket_types socket_type;
	protocols protocol;
	std::unique_ptr<endpoint> address;
};

class resolver_error : public std::runtime_error {
public:
	explicit resolver_error(const std::string & message) : std::runtime_error(message) {}
	explicit resolver_error(const char * message) : std::runtime_error(message) {}
};

std::forward_list<resolution> resolve(const std::string & host, const std::string & port, socket_types type, protocols protocol);
std::forward_list<resolution> resolve(const std::string & host, const std::string & port, protocols protocol);
std::forward_list<resolution> resolve(const std::string & host, const std::string & port, socket_types type);
std::forward_list<resolution> resolve(socket_types type, const uri & address, std::uint16_t default_port = 0);

std::unique_ptr<stream_socket_d> connect(const resolution & target, std::uint32_t flags = socket_flags::nothing);
std::unique_ptr<datagram_socket_d> prepare(const resolution & target, std::uint32_t flags = socket_flags::nothing);
std::unique_ptr<stream_server_socket_d> bind_stream(const resolution & target, std::uint32_t flags = socket_flags::nothing);
std::unique_ptr<datagram_server_socket_d> bind_datagram(const resolution & target, std::uint32_t flags = socket_flags::nothing);
std::unique_ptr<bound_socket_d> bind(const resolution & target, std::uint32_t flags = socket_flags::nothing);

template <typename something_t, typename iterator>
auto something_any(something_t something, iterator first, iterator last, std::uint32_t flags) {
	for (; first != last; ++first) {
		try {
			return something(*first, flags);
		} catch (...) { /* do nothing */ }
	}
	throw std::runtime_error("target socket is unreachable");
}

template <typename iterator>
std::unique_ptr<stream_socket_d> connect_any(iterator first, iterator last, std::uint32_t flags = socket_flags::nothing) {
	return something_any(connect, first, last, flags);
}

template <typename iterator>
std::unique_ptr<stream_server_socket_d> bind_stream_any(iterator first, iterator last, std::uint32_t flags = socket_flags::nothing) {
	return something_any(bind_stream, first, last, flags);
}

template <typename iterator>
std::unique_ptr<datagram_server_socket_d> bind_datagram_any(iterator first, iterator last, std::uint32_t flags = socket_flags::nothing) {
	return something_any(bind_datagram, first, last, flags);
}

template <typename iterator>
std::unique_ptr<bound_socket_d> bind_any(iterator first, iterator last, std::uint32_t flags = socket_flags::nothing) {
	return something_any(bind, first, last, flags);
}

} // namespace ekutils::net

#endif // RESOLVER_HEAD_QPOVFVERFS
