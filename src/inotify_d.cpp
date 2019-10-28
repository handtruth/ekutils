#include "ekutils/inotify_d.hpp"

#include <sys/inotify.h>
#include <unistd.h>

#include "ekutils/primitives.hpp"

namespace ekutils {

watch_t::~watch_t() {
	
}

void inotify_d::remove_watch(const std::unordered_map<handle_t, std::shared_ptr<watch_t>>::iterator & iter) {
	inotify_rm_watch(handle, iter->first);
	iter->second->handle = 0;
	watchers.erase(iter);
}

inotify_d::inotify_d() {
	handle = inotify_init();
	if (handle < 0)
		throw std::system_error(std::make_error_code(std::errc(errno)),
			"failed to create inotify fd");
}

inotify_d::inotify_d(inotify_d && other) : descriptor(static_cast<inotify_d &&>(other)), watchers(std::move(other.watchers)) {
	for (auto & pair : watchers)
		pair.second->fd = this;
}

const watch_t & inotify_d::add_watch(std::uint32_t mask, const fs::path & path, watch_t::data_t * data) {
	fs::path canonical = fs::weakly_canonical(path);
	int h = inotify_add_watch(handle, canonical.c_str(), mask);
	if (h < 0)
		throw std::system_error(std::make_error_code(std::errc(errno)),
			"failed to add watcher to inotify (path is \""
				+ std::string(canonical) + "\", watch mask " + std::to_string(mask) + ")");
	std::shared_ptr<watch_t> watch = std::make_shared<watch_t>(watch_t(h, canonical, mask, this));
	watch->data = data;
	watchers[h] = watch;
	return *watch;
}

const watch_t & inotify_d::mod_watch(std::uint32_t mask, const watch_t & watch) {
	if (watch.fd != this)
		throw std::invalid_argument("this watch does not belong to this inotify object");
	int h = inotify_add_watch(handle, watch.file.c_str(), mask);
	if (h < 0)
		throw std::system_error(std::make_error_code(std::errc(errno)),
			"failed to modify watch in inotify (path is \""
				+ std::string(watch.path()) + "\", new watch mask " + std::to_string(mask) + ")");
	if (h != watch.handle)
		throw std::runtime_error("FATAL new watch created! (expected modifing old one)");
	watchers[h]->mask = mask;
	return watch;
}

void inotify_d::remove_watch(const watch_t & watch) {
	auto iter = watchers.find(watch.handle);
	if (iter != watchers.end()) {
		inotify_rm_watch(handle, watch.handle);
		watchers.erase(iter);
	} else {
		throw std::invalid_argument("no such filesystem watch in inotify: " + std::string(watch.path()));
	}
}

void inotify_d::remove_watch(const fs::path & path) {
	fs::path full = fs::absolute(path);
	for (auto it = watchers.begin(), end = watchers.end(); it != end; it++)
		if (it->second->file == full) {
			inotify_rm_watch(handle, it->first);
			watchers.erase(it);
			return;
		}
	throw std::invalid_argument("no such filesystem watch in inotify: " + std::string(path));
}

const std::vector<std::shared_ptr<const watch_t>> inotify_d::subs() const noexcept {
	std::vector<std::shared_ptr<const watch_t>> result;
	for (const auto & pair : watchers)
		result.emplace_back(pair.second);
	return result;
}

std::string inotify_d::to_string() const noexcept {
	return "inotify";
}

std::vector<inotify_d::event_t> inotify_d::read() {
	static const ssize_t buff_size = 20*(sizeof(inotify_event) + 16);
	byte_t buffer[buff_size] {};
	std::vector<event_t> result;
	ssize_t length = ::read(handle, buffer, buff_size);
	if (length < 0)
		throw std::system_error(std::make_error_code(std::errc(errno)),
			"failed to read inotify fd");
	ssize_t i = 0;
	while (i < length) {
		inotify_event *event = reinterpret_cast<inotify_event *>(buffer + i);
		auto it = watchers.find(event->wd);
		if (it != watchers.end()) {
			std::shared_ptr<watch_t> watch = it->second;
			result.emplace_back(std::move(watch), event->mask, event->name);
		}
//#ifdef DEBUG
		else if (!(event->mask & inev::ignored)) {
			using namespace std::string_literals;
			throw std::runtime_error("unexpected watch {subject="s + event->name + ", mask="
				+ std::to_string(event->mask) + ", handle=" + std::to_string(event->wd) + "}");
		}
		i += sizeof(inotify_event) + event->len;
//#endif // _DEBUG
	}
	return result;
}

} // namespace ekutils
