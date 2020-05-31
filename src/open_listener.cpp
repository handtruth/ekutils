#include "open_listener.hpp"

#include <cstring>
#include <netdb.h>
#include <unistd.h>

#include "ekutils/putil.hpp"

namespace ekutils {

int open_listener(const std::string & address, const std::string & port, endpoint_info & local_info,
		int sock_type, sock_flags::flags flags) {
	addrinfo initial, *sysaddr = nullptr, *a = nullptr;
	std::memset(&initial, 0, sizeof(addrinfo));
	initial.ai_family = AF_UNSPEC;
	initial.ai_socktype = sock_type;
	initial.ai_flags = AI_PASSIVE;
	int handle;
	finnaly({
		if (sysaddr != nullptr)
			freeaddrinfo(sysaddr);
	});
	if (int s = getaddrinfo(address.c_str(), port.c_str(), &initial, &sysaddr)) {
		throw std::runtime_error("failed to create listener (getaddrinfo: " + std::string(gai_strerror(s)) + ")");
	}
	for (a = sysaddr; a != nullptr; a = a->ai_next) {
		handle = socket(a->ai_family, a->ai_socktype |
			((flags & sock_flags::non_blocking) ? SOCK_NONBLOCK : 0), a->ai_protocol);
		if (flags & sock_flags::reuse_port) {
			int opt = 1;
			setsockopt(handle, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
		}
		if (handle < 0)
			continue;
		if (!bind(handle, a->ai_addr, a->ai_addrlen))
			break;
		close(handle);
	}

	if (a == NULL)
		throw std::runtime_error("can't find any free socket to bind for \"" + address + ':' + port + '"');
	std::memcpy(&(local_info.info.addr), a->ai_addr, a->ai_addrlen);
	return handle;
}

void sockaddr2endpoint(const sockaddr * info, endpoint_info & endpoint) {
	switch (info->sa_family) {
	case AF_INET: {
		std::memcpy(&(endpoint.info.addr_in), info, sizeof(sockaddr_in));
		break;
	}
	case AF_INET6: {
		std::memcpy(&(endpoint.info.addr_in6), info, sizeof(sockaddr_in6));
		break;
	}
	case AF_UNIX: {
		std::memcpy(&(endpoint.info.addr_un), info, sizeof(sockaddr_un));
		break;
	}
	default:
		throw std::runtime_error("unexpected address family accepted: " + std::to_string(info->sa_family));
	}
}

} // namespace ekutils
