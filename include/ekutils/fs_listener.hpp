#ifndef _FS_LISTENER_HEAD
#define _FS_LISTENER_HEAD

#include <filesystem>
#include <unordered_set>
#include <vector>
#include <set>
#include <cinttypes>
#include <memory>

#include <ekutils/inotify_d.hpp>

namespace fs = std::filesystem;

namespace ekutils {

class fs_listener {
	inotify_d inotify;
public:
	enum class event_t : std::uint8_t {
		created, updated, removed
	};
	struct event {
		fs::path path;
		event_t type;
		watch_t::data_t * ud;
		event(const fs::path & p, event_t t, watch_t::data_t * u) :
			path(p), type(t), ud(u) {}
	};
private:
	enum node_kind : std::uint8_t {
		target = 1, parent = 2
	};
	fs::path croot;
	struct node_t : public watch_t::data_t {
		std::string part;
		std::uint8_t type;
		watch_t::data_t * ud;
		std::shared_ptr<node_t> parent;
		node_t(const std::string & p, std::uint8_t t, watch_t::data_t * u,
			const std::shared_ptr<node_t> & ptr) :
				part(p), type(t), ud(u), parent(ptr) {}
		const watch_t * watch = nullptr;
		std::set<std::shared_ptr<node_t>> children;
	};
	std::shared_ptr<node_t> root;
	void setup_inotify_bits(node_t & node, const fs::path & path);
	void rm_watch_recurse(node_t & node, const fs::path & path, std::vector<event> & events);
	void add_watch_recurse(node_t & node, const fs::path & path, std::vector<event> & events);
public:
	explicit fs_listener(const fs::path & path = fs::current_path());
	descriptor & d() noexcept {
		return inotify;
	}
	bool track(const fs::path & path, watch_t::data_t * ud = nullptr);
	bool forget(const fs::path & path);
	std::vector<event> fetch();
};

} // namespace ekutils

#endif // _FS_LISTENER_HEAD
