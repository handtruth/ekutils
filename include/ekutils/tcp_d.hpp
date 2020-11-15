#ifndef TCP_D_HEAD_NFH14JGKBXGFC
#define TCP_D_HEAD_NFH14JGKBXGFC

#include <variant>

#include <ekutils/socket_d.hpp>

namespace ekutils::net {

class server_tcp_socket_d;

class tcp_socket_d : public virtual bound_socket_d, public virtual ip_socket_d {
protected:
	family_t m_family = family_t::unknown;
	mutable std::variant<std::monostate, ipv4::endpoint, ipv6::endpoint> local_info;
	void open(family_t family, std::int32_t flags = socket_flags::nothing);
	tcp_socket_d() = default;
	tcp_socket_d(const tcp_socket_d & other) = delete;
	tcp_socket_d(tcp_socket_d && other) : descriptor(std::move(other)), m_family(other.m_family), local_info(other.local_info) {
		other.m_family = family_t::unknown;
		other.local_info = std::monostate();
	}
	tcp_socket_d & operator=(const tcp_socket_d & other) = delete;
	tcp_socket_d & operator=(tcp_socket_d && other) {
		descriptor::operator=(std::move(other));
		m_family = other.m_family;
		other.m_family = family_t::unknown;
		local_info = other.local_info;
		other.local_info = std::monostate();
		return *this;
	}

public:
	virtual family_t family() const noexcept final {
		return m_family;
	}
	virtual protocols protocol() const noexcept final {
		return protocols::tcp;
	}
};

class client_tcp_socket_d final : public virtual stream_socket_d, public tcp_socket_d {
	std::variant<std::monostate, ipv4::endpoint, ipv6::endpoint> remote_info;
	void connect_private(const endpoint & address);
public:
	client_tcp_socket_d() {}
	client_tcp_socket_d(client_tcp_socket_d && other) :
		descriptor(std::move(other)), tcp_socket_d(std::move(other)), remote_info(other.remote_info) {
			other.remote_info = std::monostate();
		}
	client_tcp_socket_d & operator=(client_tcp_socket_d && other) {
		descriptor::operator=(std::move(other));
		remote_info = other.remote_info;
		other.remote_info = std::monostate();
		return *this;
	}
	void connect(const ipv4::endpoint & address, std::uint32_t f = socket_flags::nothing);
	void connect(const ipv6::endpoint & address, std::uint32_t f = socket_flags::nothing);
	const endpoint * try_local_endpoint() const;
	virtual const endpoint & local_endpoint() const override;
	virtual const endpoint & remote_endpoint() const override;
	virtual std::string to_string() const noexcept override;

private:
	client_tcp_socket_d(handle_t fd, family_t family) : descriptor(fd) {
		m_family = family;
	}
	friend class server_tcp_socket_d;
};

class server_tcp_socket_d final : public virtual stream_server_socket_d, public tcp_socket_d {
	void bind_private(const endpoint & address, std::int32_t flags);
public:
	server_tcp_socket_d() {}
	server_tcp_socket_d(server_tcp_socket_d && other) : descriptor(std::move(other)), tcp_socket_d(std::move(other)) {}
	server_tcp_socket_d & operator=(server_tcp_socket_d && other) {
		tcp_socket_d::operator=(std::move(other));
		return *this;
	}
	void bind(const ipv4::endpoint & address, std::int32_t flags = socket_flags::nothing);
	void bind(const ipv6::endpoint & address, std::int32_t flags = socket_flags::nothing);
	virtual const endpoint & local_endpoint() const override;
	client_tcp_socket_d accept();
	virtual std::unique_ptr<stream_socket_d> accept_virtual() override {
		return std::make_unique<client_tcp_socket_d>(accept());
	}
	virtual std::string to_string() const noexcept override;
};

} // namespace ekutils::net

#endif // TCP_D_HEAD_NFH14JGKBXGFC
