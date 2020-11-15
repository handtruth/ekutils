#include "net_utils.hpp"

#include <cassert>

#include "sys_error.hpp"

namespace ekutils::net {

endpoint * get_endpoint(net_variant & address) {
	switch (address.index()) {
	case 0u:
		return nullptr;
	case 1u:
		return &std::get<ipv4::endpoint>(address);
	case 2u:
		return &std::get<ipv6::endpoint>(address);
	default:
		abort(); // unreachable
	}
}

const endpoint * get_endpoint(const net_variant & address) {
	switch (address.index()) {
	case 0u:
		return nullptr;
	case 1u:
		return &std::get<ipv4::endpoint>(address);
	case 2u:
		return &std::get<ipv6::endpoint>(address);
	default:
		abort(); // unreachable
	}
}

bool local_endpoint_of(endpoint & address, const socket_d & sock) {
	family_t initial = address.family();
	socklen_t len = address.sock_len();
	if (getpeername(sock.fd(), &address.sock_addr(), &len) == -1) {
		if (errno == EINPROGRESS)
			return false;
		else
			sys_error("failed to gain local endpoint");
	}
	assert(initial == address.family());
	return true;
}

void initialize_endpoint(net_variant & address, family_t family) {
	switch (family) {
		case family_t::unknown: {
			address = std::monostate();
			break;
		}
		case family_t::ipv4: {
			address = ipv4::endpoint();
			break;
		}
		case family_t::ipv6: {
			address = ipv6::endpoint();
			break;
		}
		default: {
			abort(); // unreachable
		}
	}
}

} // namespace net
