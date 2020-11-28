#ifndef IP_SPACE_HEAD_WQPVBBRVDECVC
#define IP_SPACE_HEAD_WQPVBBRVDECVC

#include <optional>

#include <ekutils/space.hpp>
#include <ekutils/socket_d.hpp>

namespace ekutils::net {

namespace ipv4 {

std::pair<address, address> interface_address(const std::string_view & name);

address mask_by_bits(unsigned bits);

std::pair<address, address> address_range(const address & addr, const address & mask);

std::pair<address, address> parse_network(const std::string_view & view);

typedef ekutils::space<address> space;

} // namespace ipv4

namespace ipv6 {

std::pair<address, address> interface_address(const std::string_view & name);

address mask_by_bits(unsigned bits);

std::pair<address, address> address_range(const address & addr, const address & mask);

std::pair<address, address> parse_network(const std::string_view & view);

typedef ekutils::space<address> space;

} // namespace ipv6

struct addresses final {
	ipv4::space ip4space;
	ipv6::space ip6space;

	bool add(const ipv4::address & target) {
		return ip4space.insert(target).second;
	}
	bool add(const ipv6::address & target) {
		return ip6space.insert(target).second;
	}
	bool add(const std::string_view & str);

	bool contains(const endpoint & target) const;
	bool contains(const ipv4::address & target) const {
		return ip4space.contains(target);
	}
	bool contains(const ipv6::address & target) const {
		return ip6space.contains(target);
	}

	bool empty() const {
		return ip4space.empty() && ip6space.empty();
	}
};

} // namespace ekutils::net

#endif // IP_SPACE_HEAD_WQPVBBRVDECVC
