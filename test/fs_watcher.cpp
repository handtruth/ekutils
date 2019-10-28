#include <fstream>
#include <filesystem>
#include <ekutils/fs_listener.hpp>

namespace std {
    string to_string(const ekutils::fs_listener::event_t & et) {
        return to_string(int(et));
    }
}

#include "test.hpp"

test {
    using namespace ekutils;
    fs_listener fsl;
    assert_true(fsl.track("fsl_dir/file1.txt"));
    assert_true(fsl.track("fsl_dir/subdir/file2.txt"));
    assert_false(fsl.track("fsl_dir/file1.txt"));
    assert_false(fsl.track("fsl_dir/subdir/file2.txt"));
    assert_true(fsl.track("fsl_dir/subdir"));
    
    fs::create_directory("fsl_dir");
    auto changes = fsl.fetch();
    assert_true(changes.empty());

    std::ofstream("fsl_dir/file3.txt") << "";
    changes = fsl.fetch();
    assert_true(changes.empty());

    std::ofstream("fsl_dir/file1.txt") << "";
    changes = fsl.fetch();
    assert_equals(1u, changes.size());
    assert_equals(fs_listener::event_t::created, changes.front().type);
    assert_equals("fsl_dir/file1.txt", changes.front().path);
    
    fs::create_directory("fsl_dir/subdir");
    changes = fsl.fetch();
    assert_equals(1u, changes.size());
    assert_equals(fs_listener::event_t::created, changes.front().type);
    assert_equals("fsl_dir/subdir", changes.front().path);
    
    std::ofstream("fsl_dir/subdir/file2.txt") << "1";
    changes = fsl.fetch();
    assert_equals(1u, changes.size());
    assert_equals(fs_listener::event_t::created, changes.back().type);
    assert_equals("fsl_dir/subdir/file2.txt", changes.front().path);

    std::ofstream("fsl_dir/subdir/file2.txt") << "2";
    changes = fsl.fetch();
    assert_equals(1u, changes.size());
    assert_equals(fs_listener::event_t::updated, changes.back().type);
    assert_equals("fsl_dir/subdir/file2.txt", changes.front().path);

    fs::rename("fsl_dir", "fsl_dir2");
    changes = fsl.fetch();
    assert_equals(3u, changes.size());
    for (const auto & each : changes) {
        assert_equals(fs_listener::event_t::removed, each.type);
    }

    fs::rename("fsl_dir2", "fsl_dir");
    changes = fsl.fetch();
    assert_equals(3u, changes.size());
    for (const auto & each : changes) {
        assert_equals(fs_listener::event_t::created, each.type);
    }

    fs::remove_all("fsl_dir");
    changes = fsl.fetch();
    assert_equals(3u, changes.size());
    for (const auto & each : changes) {
        assert_equals(fs_listener::event_t::removed, each.type);
    }
    int g = 0;
    std::cout << g;
}
