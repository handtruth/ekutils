#ifndef RESOLVER_HEAD_QPOVFVERFS
#define RESOLVER_HEAD_QPOVFVERFS

#include <forward_list>

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
std::forward_list<resolution> resolve(socket_types type, const uri & address);

std::unique_ptr<stream_socket_d> connect(const resolution & target, std::uint32_t flags = socket_flags::nothing);
std::unique_ptr<datagram_socket_d> prepare(const resolution & target, std::uint32_t flags = socket_flags::nothing);
std::unique_ptr<stream_server_socket_d> bind_stream(const resolution & target, std::uint32_t flags = socket_flags::nothing);
std::unique_ptr<datagram_server_socket_d> bind_datagram(const resolution & target, std::uint32_t flags = socket_flags::nothing);
std::unique_ptr<bound_socket_d> bind(const resolution & target, std::uint32_t flags = socket_flags::nothing);

} // namespace ekutils::net

#endif // RESOLVER_HEAD_QPOVFVERFS
