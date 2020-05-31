#ifndef OPEN_LISTENER_HEAD_MHGREFGDHYFHGF
#define OPEN_LISTENER_HEAD_MHGREFGDHYFHGF

#include <string>

#include "ekutils/connection_info.hpp"

namespace ekutils {

int open_listener(const std::string & address, const std::string & port, endpoint_info & local_info,
		int sock_type, sock_flags::flags flags);

void sockaddr2endpoint(const sockaddr * info, endpoint_info & endpoint);

} // namespace ekutils

#endif // OPEN_LISTENER_HEAD_MHGREFGDHYFHGF
