#ifndef NET_UTILS_HEAD_WDDREHNYJTNBR
#define NET_UTILS_HEAD_WDDREHNYJTNBR

#include <variant>

#include "ekutils/socket_d.hpp"

namespace ekutils::net {

typedef std::variant<std::monostate, ipv4::endpoint, ipv6::endpoint> net_variant;

endpoint * get_endpoint(net_variant & address);
const endpoint * get_endpoint(const net_variant & address);

bool local_endpoint_of(endpoint & address, const socket_d & sock);

void initialize_endpoint(net_variant & address, family_t family);

} // namespace net

#endif // NET_UTILS_HEAD_WDDREHNYJTNBR
