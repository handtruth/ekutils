#include "ekutils/listener_socket_d.hpp"

namespace ekutils {

void listener_socket_d::start(int backlog) {
	if (::listen(handle, backlog) == -1)
		throw std::system_error(std::make_error_code(std::errc(errno)), "failed to start tcp listener");
	socklen_t len = local_info.addr_len();
	if (getsockname(handle, &local_info.info.addr, &len) == -1)
		throw std::system_error(std::make_error_code(std::errc(errno)), "failed to gain info about tcp socket");
}

void listener_socket_d::set_reusable(bool reuse) {
	int opt = reuse ? 1 : 0;
	if (setsockopt(handle, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1) {
		throw std::system_error(std::make_error_code(std::errc(errno)), "can't set SO_REUSEPORT option for tcp listener");
	}
}

} // namespace ekutils
