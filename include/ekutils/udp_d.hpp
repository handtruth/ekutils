#ifndef UDP_D_HEAD_OLU89HE3W76BNWDA
#define UDP_D_HEAD_OLU89HE3W76BNWDA

#include <variant>

#include <ekutils/socket_d.hpp>

namespace ekutils::net {

class udp_socket_d : public virtual datagram_socket_d, public virtual ip_socket_d {
	family_t m_family = family_t::unknown;
protected:
	void open(family_t family, std::int32_t flags = socket_flags::nothing);
	udp_socket_d() = default;
	udp_socket_d(const udp_socket_d & other) = delete;
	udp_socket_d(udp_socket_d && other) : descriptor(std::move(other)), m_family(other.m_family) {
		other.m_family = family_t::unknown;
	}
	udp_socket_d & operator=(const udp_socket_d & other) = delete;
	udp_socket_d & operator=(udp_socket_d && other) {
		descriptor::operator=(std::move(other));
		m_family = other.m_family;
		other.m_family = family_t::unknown;
		return *this;
	}

public:
	virtual family_t family() const noexcept final {
		return m_family;
	}
	virtual protocols protocol() const noexcept final {
		return protocols::udp;
	}
};

struct client_udp_socket_d final : public udp_socket_d {
	client_udp_socket_d() = default;
	client_udp_socket_d(client_udp_socket_d && other) : descriptor(std::move(other)), udp_socket_d(std::move(other)) {}
	client_udp_socket_d & operator=(client_udp_socket_d && other) {
		udp_socket_d::operator=(std::move(other));
		return *this;
	}
	using udp_socket_d::open;
	virtual std::string to_string() const noexcept override;
};

class server_udp_socket_d final : public udp_socket_d, public virtual datagram_server_socket_d {
	std::variant<std::monostate, ipv4::endpoint, ipv6::endpoint> local_info;
	void bind_private(const endpoint & address, std::int32_t flags);
public:
	server_udp_socket_d() = default;
	server_udp_socket_d(server_udp_socket_d && other) :
		descriptor(std::move(other)), udp_socket_d(std::move(other)), local_info(other.local_info) {
			other.local_info = std::monostate();
		}
	server_udp_socket_d & operator=(server_udp_socket_d && other) {
		udp_socket_d::operator=(std::move(other));
		local_info = other.local_info;
		other.local_info = std::monostate();
		return *this;
	}
	void bind(const ipv4::endpoint & address, std::int32_t flags = socket_flags::nothing);
	void bind(const ipv6::endpoint & address, std::int32_t flags = socket_flags::nothing);
	virtual const endpoint & local_endpoint() const override;
	virtual std::string to_string() const noexcept override;
};

} // namespace ekutils::net

#endif // UDP_D_HEAD_OLU89HE3W76BNWDA
