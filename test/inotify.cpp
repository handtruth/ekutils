#include "test.hpp"

#include "ekutils/epoll_d.hpp"
#include "ekutils/inotify_d.hpp"

#include <filesystem>

ekutils::inotify_d fs_event;

test {
    using namespace ekutils;
    namespace fs = std::filesystem;
    epoll_d pull;
    fs_event.add_watch(inev::create, ".");
    pull.add(fs_event, actions::in, [](descriptor & f, std::uint32_t) {
        assert_equals(fs_event.to_string(), f.to_string());
        inotify_d & inotify = dynamic_cast<inotify_d &>(f);
        auto whats = inotify.read();
        assert_equals(std::size_t(1), whats.size());
        assert_equals(fs::canonical("."), whats[0].watch.path());
        assert_equals(inev::create, inev::create & whats[0].mask);
    });
    fs::create_directory("inotify.d");
    pull.wait(1000);
    fs::remove_all("inotify.d");
}
