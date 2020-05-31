#include "ekutils/connection_info.hpp"

#include <cstring>
#include <netdb.h>

#include "ekutils/putil.hpp"

namespace ekutils {

endpoint_info endpoint_info::empty = endpoint_info();

std::string endpoint_info::address() const {
	switch (info.addr.sa_family) {
	case AF_INET: {
		const int length = 20;
		char buffer[length];
		inet_ntop(AF_INET, &(info.addr_in.sin_addr), buffer, length);
		return buffer;
	}
	case AF_INET6: {
		const int length = 256;
		char buffer[length];
		inet_ntop(AF_INET6, &(info.addr_in6.sin6_addr), buffer, length);
		return buffer;
	}
	case AF_UNIX: {
		return info.addr_un.sun_path;
	}
	default:
		throw std::runtime_error("unexpected address family accepted: " + std::to_string(info.addr.sa_family));
	}
}

std::vector<connection_info> connection_info::resolve(const std::string & address, const std::string & port) {
	addrinfo initial, *sysaddr = nullptr, *a = nullptr;
	std::memset(&initial, 0, sizeof(addrinfo));
	initial.ai_family = AF_UNSPEC;
	initial.ai_socktype = SOCK_STREAM;
	
	finnaly({
		if (sysaddr != nullptr)
			freeaddrinfo(sysaddr);
	});

	if (int s = getaddrinfo(address.c_str(), port.c_str(), &initial, &sysaddr)) {
		throw dns_error("failed to resolve socket address \"" + address + ':' + port + "\" (getaddrinfo: " + std::string(gai_strerror(s)) + ")");
	}
	std::vector<connection_info> result;
	for (a = sysaddr; a != nullptr; a = a->ai_next) {
		connection_info & info = result.emplace_back();
		std::memcpy(&(info.endpoint.info.addr), a->ai_addr, a->ai_addrlen);
		info.protocol = a->ai_protocol;
		info.sock_type = a->ai_socktype;
	}
	result.shrink_to_fit();
	return result;
}

} // namespace ekutils
