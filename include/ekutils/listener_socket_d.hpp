#ifndef LISTENER_SOCKET_D_HEAD_QQPNEVYUFUUN
#define LISTENER_SOCKET_D_HEAD_QQPNEVYUFUUN

#include <memory>

#include <ekutils/stream_socket_d.hpp>

namespace ekutils {

class listener_socket_d : public descriptor {
protected:
	endpoint_info local_info;
public:
	void start(int backlog = 25);
	const endpoint_info & local_endpoint() const noexcept {
		return local_info;
	}
	virtual std::unique_ptr<stream_socket_d> accept_virtual() = 0;

	void set_reusable(bool reuse = true);
};

} // namespace ekutils

#endif // LISTENER_SOCKET_D_HEAD_QQPNEVYUFUUN
