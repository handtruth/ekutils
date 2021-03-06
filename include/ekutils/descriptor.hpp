#ifndef _DESCRIPTOR_HEAD
#define _DESCRIPTOR_HEAD

#include <string>
#include <cinttypes>
#include <memory>

namespace ekutils {

typedef int handle_t;

class epoll_d;

class descriptor {
public:
	bool operator==(const descriptor & other) const noexcept {
		return handle == other.handle;
	}
	operator bool() const noexcept {
		return handle != -1;
	}
	void set_non_block(bool nonblock = true);
	std::size_t avail() const;
	virtual std::string to_string() const noexcept = 0;
	void close();
	virtual ~descriptor();
private:
	struct record_base {
		descriptor * efd;
		record_base(record_base && other) = delete;
		record_base(const record_base & other) = delete;
		record_base(descriptor & fd) : efd(&fd) {}
		virtual void operator()(std::uint32_t events) = 0;
		virtual ~record_base();
	};
	template <typename F>
	struct record_t : public record_base {
		F act;
		record_t(descriptor & fd, F action) : record_base(fd), act(action) {}
		virtual void operator()(std::uint32_t events) override {
			act(*efd, events);
		}
	};
	std::unique_ptr<record_base> record;
protected:
	void check_created() const;
	descriptor() : handle(-1) {}
	handle_t handle;
	descriptor(descriptor && other) {
		handle = other.handle;
		other.handle = -1;
		record = std::move(other.record);
		if (record) {
			record->efd = this;
		}
	}
	constexpr explicit descriptor(handle_t fd) : handle(fd) {}
	descriptor & operator=(descriptor && other) {
		close();
		handle = other.handle;
		other.handle = -1;
		record = std::move(other.record);
		if (record) {
			record->efd = this;
		}
		return *this;
	}
public:
	handle_t fd() const noexcept { return handle; }
	friend class epoll_d;
};

} // namespace ekutils

namespace std {

inline string to_string(ekutils::descriptor & fd) {
	return fd.to_string();
}

} // namespace std

#endif // _DESCRIPTOR_HEAD
