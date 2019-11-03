#include "ekutils/fs_listener.hpp"

#include <algorithm>
#include <cassert>

namespace ekutils {

void fs_listener::setup_inotify_bits(node_t & node, const fs::path & path) {
	if (!fs::exists(path)) {
		node.watch = nullptr;
		return;
	}
	std::uint32_t bits = 0;
	if (node.type & parent) {
		using namespace inev;
		bits |= delete_self | create | moved_to | move_self;
	}
	if (node.type & target) {
		using namespace inev;
		if (!fs::is_directory(path))
			bits |= close_write | create | moved_to;
		bits |= delete_self | move_self;
	}
	if (node.watch)
		node.watch = &inotify.mod_watch(bits, *node.watch);
	else
		node.watch = &inotify.add_watch(bits, path, &node);
}

void fs_listener::rm_watch_recurse(node_t & node, const fs::path & path, std::vector<event> & events) {
	if (node.watch) {
		if (node.type & target)
			events.emplace_back(path, event_t::removed, node.ud);
		inotify.remove_watch(*node.watch);
		node.watch = nullptr;
		for (auto & next : node.children)
			rm_watch_recurse(*next, path/next->part, events);
	}
}

void fs_listener::add_watch_recurse(node_t & node, const fs::path & path, std::vector<event> & events) {
	assert(!node.watch);
	if (fs::exists(path)) {
		if (node.type & target) {
			if (fs::is_directory(path)) {
				for (const auto & entry : fs::directory_iterator(path))
					events.emplace_back(path/entry.path().filename(),
						event_t::created_in, node.ud);
			}
			events.emplace_back(path, event_t::created, node.ud);
		}
		setup_inotify_bits(node, path);
		for (auto & child : node.children)
			add_watch_recurse(*child, path/child->part, events);
	}
}

void fs_listener::collect_target_recurse(const node_t & node, const fs::path & path, std::vector<fs::path> & result) {
	fs::path cpath = path/node.part;
	if (node.type & target) {
		result.push_back(cpath);
	}
	for (const auto & next : node.children)
		collect_target_recurse(*next, cpath, result);
}

fs_listener::fs_listener(const fs::path & path) :
		croot(fs::canonical(path)), root(std::make_shared<node_t>(".", parent, nullptr, nullptr)) {
	root->watch = &inotify.add_watch(inev::create | inev::moved_to, path, root.get());
}

bool fs_listener::track(const fs::path & path, watch_t::data_t * ud) {
	fs::path relative = fs::relative(fs::absolute(path), croot);
	if (*relative.begin() == "..")
		throw std::runtime_error("only files in sub directories can be tracked to");
	if (relative == ".") {
		root->ud = ud;
		if (root->type & target) {
			return false;
		} else {
			root->type |= target;
			return true;
		}
	}
	std::shared_ptr<node_t> cnode = root;
	fs::path cpath = croot;
	for (auto itpart = relative.begin(), endpart = --relative.end(); itpart != endpart; ++itpart) {
		const fs::path & part = *itpart;
		auto & cchildren = cnode->children;
		auto iter = std::find_if(cchildren.begin(), cchildren.end(),
			[&part](const auto & it) -> bool {
				return it->part == part;
			}
		);
		if (iter != cnode->children.end()) {
			auto & next_node = *iter;
			cpath /= next_node->part;
			if (!(next_node->type & parent)) {
				next_node->type |= parent;
				setup_inotify_bits(*next_node, cpath);
			}
			cnode = next_node;
		} else {
			auto next_node_it = cchildren.emplace(std::make_shared<node_t>(part, parent, nullptr, cnode));
			assert(next_node_it.second);
			auto & next_node = *next_node_it.first;
			cpath /= next_node->part;
			setup_inotify_bits(*next_node, cpath);
			cnode = next_node;
		}
	}
	auto & cchildren = cnode->children;
	const fs::path & name = relative.filename();
	cpath /= name;
	auto iter = std::find_if(cchildren.begin(), cchildren.end(),
		[&name](const auto & it) -> bool {
			return it->part == name;
		}
	);
	if (iter != cchildren.end()) {
		auto & next_node = *iter;
		next_node->ud = ud;
		if (next_node->type & target) {
			return false;
		} else {
			next_node->type |= target;
			setup_inotify_bits(*next_node, cpath);
			return true;
		}
	} else {
		auto next_node_it = cchildren.emplace(std::make_shared<node_t>(name, target, ud, cnode));
		assert(next_node_it.second);
		auto & next_node = *next_node_it.first;
		setup_inotify_bits(*next_node, cpath);
		return true;
	}
}

bool fs_listener::forget(const fs::path & path) {
	fs::path relative = fs::relative(path, croot);
	if (*relative.begin() == "..")
		throw std::runtime_error("stop!");
	if (relative == ".") {
		if (root->type & target) {
			root->type &= ~target;
			root->ud = nullptr;
			return true;
		} else {
			return false;
		}
	}
	std::shared_ptr<node_t> cnode = root;
	for (const fs::path & part : path) {
		auto & cchildren = cnode->children;
		auto iter = std::find_if(cchildren.begin(), cchildren.end(),
			[&part](const auto & it) -> bool {
				return it->part == part;
			}
		);
		if (iter != cchildren.end()) {
			cnode = *iter;
		} else {
			return false;
		}
	}
	cnode->type &= ~target;
	std::shared_ptr<node_t> parent;
	while (!(cnode->type & target) && !cnode->children.size() && (parent = cnode->parent.lock())) {
		assert(parent);
		parent->children.erase(cnode);
		if (cnode->watch)
			inotify.remove_watch(*cnode->watch);
		cnode = parent;
	}
	return true;
}

std::vector<fs::path> fs_listener::targets() const {
	std::vector<fs::path> result;
	fs::path path = ".";
	if (root->type & target) {
		result.push_back(path);
	}
	for (const auto & next : root->children)
		collect_target_recurse(*next, path, result);
	return result;
}

std::vector<fs_listener::event> fs_listener::fetch() {
	auto events = inotify.read();
	std::vector<event> result;
	for (auto iter = events.rbegin(); iter != events.rend(); ++iter) {
		auto & e = *iter;
		node_t & node = *dynamic_cast<node_t *>(e.watch.data);
		if (e.mask & inev::close_write) {
			result.emplace_back(fs::relative(e.watch.path(), croot), event_t::updated, node.ud);
		}
		if (e.mask & (inev::delete_self | inev::move_self)) {
			rm_watch_recurse(node, fs::relative(e.watch.path(), croot), result);
		}
		if (e.mask & (inev::create | inev::moved_to)) {
			fs::path relative = fs::relative(e.watch.path(), croot);
			if (node.type & target)
				result.emplace_back(relative/e.subject, event_t::created_in, node.ud);
			auto search = std::find_if(node.children.begin(), node.children.end(),
				[&e](const auto & it) -> bool {
					return it->part == e.subject;
				}
			);
			if (search != node.children.end()) {
				auto & next = **search;
				add_watch_recurse(next, relative == "." ? fs::path(next.part) : relative/next.part, result);
			}
		}
	}
	return result;
}

fs_listener::~fs_listener() {}

} // namespace ekutils
