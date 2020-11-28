#include "ekutils/ip_space.hpp"

#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#include "ekutils/resolver.hpp"
#include "ekutils/finally.hpp"
#include "sys_error.hpp"

namespace ekutils::net {

inline ipv4::address address_of(const sockaddr_in & addr) {
	return addr.sin_addr.s_addr;
}

inline ipv6::address address_of(const sockaddr_in6 & addr) {
	return addr.sin6_addr;
}

template <typename address_my, typename address_glib, int domain>
std::pair<address_my, address_my> interface_address_base(const std::string_view & name) {
	if (name.size() > IFNAMSIZ - 1)
		throw std::invalid_argument("network interface name is too big");
	int fd = socket(domain, SOCK_DGRAM, 0);
	if (fd == -1)
		sys_error("failed to open a dummy socket");
	finally({
		close(fd);
	});
	ifreq ifr;
	ifr.ifr_addr.sa_family = domain;
	std::strncpy(ifr.ifr_name, name.data(), name.size());
	ifr.ifr_name[name.size()] = '\0';
	if (ioctl(fd, SIOCGIFADDR, &ifr) == -1)
		sys_error("failed to get an address of the interface");
	address_my addr = address_of(reinterpret_cast<const address_glib &>(ifr.ifr_addr));
	if (ioctl(fd, SIOCGIFNETMASK, &ifr) == -1)
		sys_error("failed to get a network mask of the interface");
	address_my mask = address_of(reinterpret_cast<const address_glib &>(ifr.ifr_netmask));
	return std::make_pair(addr, mask);
}

namespace ipv4 {

std::pair<address, address> interface_address(const std::string_view & name) {
	return interface_address_base<address, sockaddr_in, AF_INET>(name);
}

address mask_by_bits(unsigned bits) {
	if (bits >= 32)
		throw std::invalid_argument("mask bits count is too long");
	address result;
	result.data = 0;
	for (unsigned i = 31; i >= 32 - bits; --i) {
		result.data |= 1u << i;
	}
	result.data = hton(result.data);
	return result;
}

std::pair<address, address> address_range(const address & addr, const address & mask) {
	address start = addr.data & mask.data;
	address end = start.data | ~mask.data;
	return std::make_pair(start, end);
}

std::pair<address, address> parse_network(const std::string_view & view) {
	constexpr std::size_t limit = std::numeric_limits<std::size_t>::max();
	if (!view.empty() && view[0] == '*') {
		std::pair result = interface_address(view.substr(1));
		return address_range(result.first, result.second);
	}
	auto slash = view.find('/');
	if (slash != limit) {
		address addr(view.substr(0, slash));
		auto bits = std::stoul(std::string(view.substr(slash + 1)));
		address mask = mask_by_bits(static_cast<unsigned>(bits));
		return address_range(addr, mask);
	}
	auto dash = view.find('-');
	if (dash != limit) {
		address start(view.substr(0, dash));
		address end(view.substr(dash + 1));
		return std::make_pair(start, end);
	}
	address addr(view);
	return std::make_pair(addr, addr);
}

} // namespace ipv4

namespace ipv6 {

std::pair<address, address> interface_address(const std::string_view & name) {
	return interface_address_base<address, sockaddr_in6, AF_INET6>(name);
}

address mask_by_bits(unsigned bits) {
	if (bits >= 128)
		throw std::invalid_argument("mask bits count is too long");
	address result;
	result.data.fill(0);
	unsigned ones = bits >> 3;
	unsigned i;
	for (i = 0; i < ones; ++i) {
		result.data[i] = 0xffu;
	}
	std::uint8_t octet = 0;
	unsigned cnt = bits & 7;
	for (unsigned j = 7; j >= 8 - cnt; --j) {
		octet |= 1 << j;
	}
	result.data[i] = octet;
	return result;
}

std::pair<address, address> address_range(const address & addr, const address & mask) {
	address start = addr;
	for (unsigned i = 0; i < 16; ++i) {
		start.data[i] &= mask.data[i];
	}
	address end = start;
	for (unsigned i = 0; i < 16; ++i) {
		end.data[i] |= ~mask.data[i];
	}
	return std::make_pair(start, end);
}

std::pair<address, address> parse_network(const std::string_view & view) {
	constexpr std::size_t limit = std::numeric_limits<std::size_t>::max();
	if (!view.empty() && view[0] == '*') {
		std::pair result = interface_address(view.substr(1));
		return address_range(result.first, result.second);
	}
	auto slash = view.find('/');
	if (slash != limit) {
		address addr(view.substr(0, slash));
		auto bits = std::stoul(std::string(view.substr(slash + 1)));
		address mask = mask_by_bits(static_cast<unsigned>(bits));
		return address_range(addr, mask);
	}
	auto dash = view.find('-');
	if (dash != limit) {
		address start(view.substr(0, dash));
		address end(view.substr(dash + 1));
		return std::make_pair(start, end);
	}
	address addr(view);
	return std::make_pair(addr, addr);
}

typedef ekutils::space<address> space;

} // namespace ipv6

bool addresses::add(const std::string_view & str) {
	auto slash = str.find('/');
	auto dash = str.find('-');
	auto delimeter = std::min(slash, dash);
	int variant;
	constexpr int ip4network = 0;
	constexpr int ip6network = 1;
	constexpr int name = 2;
	constexpr int ifname = 3;
	try {
		ipv4::address(str.substr(0, delimeter));
		variant = ip4network;
	} catch (const std::invalid_argument &) {
		try {
			ipv6::address(str.substr(0, delimeter));
			variant = ip6network;
		} catch (const std::invalid_argument &) {
			if (!str.empty() && str[0] == '*')
				variant = ifname;
			else
				variant = name;
		}
	}
	switch (variant) {
		case ip4network: {
			auto range = ipv4::parse_network(str);
			return ip4space.insert(range.first, range.second).second;
		}
		case ip6network: {
			auto range = ipv6::parse_network(str);
			return ip6space.insert(range.first, range.second).second;
		}
		case name: {
			auto targets = resolve(std::string(str), "0", socket_types::unknown, protocols::unknown);
			bool result = false;
			for (const auto & target : targets) {
				switch (target.address->family()) {
					case family_t::ipv4: {
						const auto & addr = dynamic_cast<const ipv4::endpoint &>(*target.address);
						result = result || ip4space.insert(addr.address()).second;
						break;
					}
					case family_t::ipv6: {
						const auto & addr = dynamic_cast<const ipv6::endpoint &>(*target.address);
						result = result || ip6space.insert(addr.address()).second;
						break;
					}
					default: break;
				}
			}
			return result;
		}
		case ifname: {
			bool result = false;
			try {
				std::pair addr = ipv4::interface_address(str.substr(1));
				std::pair range = ipv4::address_range(addr.first, addr.second);
				result = ip4space.insert(range.first, range.second).second;
			} catch (const std::system_error &) {
				/* do nothing */
			}
			try {
				std::pair addr = ipv6::interface_address(str.substr(1));
				std::pair range = ipv6::address_range(addr.first, addr.second);
				result = result || ip6space.insert(range.first, range.second).second;
			} catch (const std::system_error &) {
				/* do nothing */
			}
			return result;
		}
		default: abort(); // unreachable;
	}
}

bool addresses::contains(const endpoint & target) const {
	switch (target.family()) {
	case family_t::ipv4: {
		const auto & addr = dynamic_cast<const ipv4::endpoint &>(target);
		return ip4space.contains(addr.address());
	}
	case family_t::ipv6: {
		const auto & addr = dynamic_cast<const ipv6::endpoint &>(target);
		return ip6space.contains(addr.address());
	}
	default: return false;
	}
}

} // namespace ekutils::net
