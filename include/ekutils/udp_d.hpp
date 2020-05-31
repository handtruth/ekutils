#ifndef UDP_D_HEAD_OLU89HE3W76BNWDA
#define UDP_D_HEAD_OLU89HE3W76BNWDA

#include <ekutils/connection_info.hpp>
#include <ekutils/in_stream_d.hpp>
#include <ekutils/out_stream_d.hpp>

namespace ekutils {

class udp_server_d : public descriptor {
private:
	sock_flags::flags flags;
	endpoint_info local_info;
public:
	udp_server_d(const std::string & address, const std::string & port, sock_flags::flags f = sock_flags::nothing);
	virtual std::string to_string() const noexcept override;
	int read(byte_t bytes[], size_t length, endpoint_info * remote_endpoint);
	int write(const byte_t bytes[], size_t length, const endpoint_info & remote_endpoint);
	const endpoint_info & local_endpoint() const noexcept {
		return local_info;
	}
	virtual ~udp_server_d() override;
};

} // namespace ekutils

#endif // UDP_D_HEAD_OLU89HE3W76BNWDA
