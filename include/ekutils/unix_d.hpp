#ifndef UNIX_D_HEAD_YDBQTU6FTRS
#define UNIX_D_HEAD_YDBQTU6FTRS

#include <optional>

#include <ekutils/socket_d.hpp>

namespace ekutils::net {

class unix_stream_listener_d;
class unix_stream_socket_d;

struct unix_socket_d : public virtual socket_d {
	virtual family_t family() const noexcept final {
		return family_t::un;
	}
	unix_socket_d() = default;
	unix_socket_d(const unix_socket_d & other) = delete;
	unix_socket_d & operator=(const unix_socket_d & other) = delete;
};

struct datagram_unix_socket_d : public unix_socket_d, public virtual datagram_socket_d {
protected:
	void open(std::int32_t flags = socket_flags::nothing);
};

struct client_datagram_unix_socket_t final : public datagram_unix_socket_d {
	client_datagram_unix_socket_t() = default;
	client_datagram_unix_socket_t(client_datagram_unix_socket_t && other) :
		descriptor(std::move(other)) {}
	client_datagram_unix_socket_t & operator=(client_datagram_unix_socket_t && other) {
		descriptor::operator=(std::move(other));
		return *this;
	}
	using datagram_unix_socket_d::open;
	virtual std::string to_string() const noexcept override;
};

class server_datagram_unix_socket_d final : public datagram_unix_socket_d, public datagram_server_socket_d {
	un::endpoint info;

public:
	server_datagram_unix_socket_d() = default;
	server_datagram_unix_socket_d(server_datagram_unix_socket_d && other) :
		descriptor(std::move(other)), info(other.info) {}
	server_datagram_unix_socket_d & operator=(server_datagram_unix_socket_d && other) {
		descriptor::operator=(std::move(other));
		info = other.info;
		return *this;
	}
	void bind(const un::endpoint & address, std::int32_t flags = socket_flags::nothing);
	virtual const endpoint & local_endpoint() const override;
	virtual std::string to_string() const noexcept override;
};

struct stream_unix_socket_d : public unix_socket_d, public virtual bound_socket_d {
protected:
	un::endpoint info;
	void open(std::int32_t flags = socket_flags::nothing);
	stream_unix_socket_d() = default;
	stream_unix_socket_d(stream_unix_socket_d && other) :
		descriptor(std::move(other)), info(other.info) {}
	stream_unix_socket_d & operator=(stream_unix_socket_d && other) {
		descriptor::operator=(std::move(other));
		info = other.info;
		return *this;
	}
	virtual const endpoint & local_endpoint() const override;
};

struct client_stream_unix_socket_d final : public stream_unix_socket_d, public stream_socket_d {
	client_stream_unix_socket_d() {}
	client_stream_unix_socket_d(client_stream_unix_socket_d && other) :
		descriptor(std::move(other)), stream_unix_socket_d(std::move(other)) {}
	client_stream_unix_socket_d & operator=(client_stream_unix_socket_d && other) {
		stream_unix_socket_d::operator=(std::move(other));
		return *this;
	}
	void connect(const un::endpoint & address, std::uint32_t f = socket_flags::nothing);
	virtual const endpoint & local_endpoint() const override;
	virtual const endpoint & remote_endpoint() const override;
	virtual std::string to_string() const noexcept override;

private:
	client_stream_unix_socket_d(handle_t fd) : descriptor(fd) {}
	friend struct server_stream_unix_socket_d;
};

struct server_stream_unix_socket_d final : public stream_unix_socket_d, public stream_server_socket_d {
	server_stream_unix_socket_d() {}
	server_stream_unix_socket_d(server_stream_unix_socket_d && other) :
		descriptor(std::move(other)), stream_unix_socket_d(std::move(other)) {}
	server_stream_unix_socket_d & operator=(server_stream_unix_socket_d && other) {
		stream_unix_socket_d::operator=(std::move(other));
		return *this;
	}
	void bind(const un::endpoint & address, std::int32_t flags = socket_flags::nothing);
	virtual const endpoint & local_endpoint() const override;
	client_stream_unix_socket_d accept();
	virtual std::unique_ptr<stream_socket_d> accept_virtual() override {
		return std::make_unique<client_stream_unix_socket_d>(accept());
	}
	virtual std::string to_string() const noexcept override;
};

} // namespace ekutils::net

#endif // UNIX_D_HEAD_YDBQTU6FTRS
