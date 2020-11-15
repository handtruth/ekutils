#include <fstream>
#include <filesystem>
#include <ekutils/fs_listener.hpp>
#include <ekutils/finally.hpp>

namespace std {
	string to_string(const ekutils::fs_listener::event_t & et) {
		return to_string(int(et));
	}
    string to_string(const ekutils::watch_t::data_t * ptr) {
        char str[256];
        std::sprintf(str, "%p", reinterpret_cast<const void *>(ptr));
        return str;
    }
}

#include "test.hpp"

test {
	using namespace ekutils;
	fs_listener fsl;
	struct : watch_t::data_t {} file1, file2, subdir;
	assert_true(fsl.track("fsl_dir/file1.txt", &subdir));
	assert_true(fsl.track("fsl_dir/subdir/file2.txt", &file1));
	assert_true(fsl.forget("fsl_dir/subdir/file2.txt"));
	assert_true(fsl.track("fsl_dir/subdir/file2.txt", nullptr));
	assert_false(fsl.track("fsl_dir/file1.txt", &file1));
	assert_true(fsl.track("fsl_dir/subdir", &file2));
	assert_true(fsl.forget("fsl_dir/subdir"));
	assert_true(fsl.track("fsl_dir/subdir", &subdir));
	assert_false(fsl.track("fsl_dir/subdir/file2.txt", &file2));

	fs::create_directory("fsl_dir");
	{
	finally({
		fs::remove_all("fsl_dir");
		if (fs::exists("fsl_dir2"))
			fs::remove_all("fsl_dir2");
	});
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
	assert_equals(2u, changes.size());
	for (const auto & each : changes) {
		assert_any({
			assert_equals(fs_listener::event_t::created, each.type);
			assert_equals(&file2, each.ud);
		} assert_or {
			assert_equals(fs_listener::event_t::created_in, each.type);
			assert_equals(&subdir, each.ud);
		});
		assert_equals("fsl_dir/subdir/file2.txt", each.path);
	}

	std::ofstream("fsl_dir/subdir/file2.txt") << "2";
	changes = fsl.fetch();
	assert_equals(1u, changes.size());
	assert_equals(fs_listener::event_t::updated, changes.back().type);
	assert_equals("fsl_dir/subdir/file2.txt", changes.front().path);

	fs::rename("fsl_dir", "fsl_dir2");
	changes = fsl.fetch();
	assert_equals(3u, changes.size());
	for (const auto & each : changes) {
		assert_any({
			assert_equals("fsl_dir/subdir/file2.txt", each.path);
			assert_equals(&file2, each.ud);
		} assert_or {
			assert_equals("fsl_dir/subdir", each.path);
			assert_equals(&subdir, each.ud);
		} assert_or {
			assert_equals("fsl_dir/file1.txt", each.path);
			assert_equals(&file1, each.ud);
		});
		assert_equals(fs_listener::event_t::removed, each.type);
	}

	fs::rename("fsl_dir2", "fsl_dir");
	changes = fsl.fetch();
	assert_equals(4u, changes.size());
	for (const auto & each : changes) {
		using et = fs_listener::event_t;
		assert_any({
			assert_equals("fsl_dir/subdir", each.path);
			assert_equals(et::created, each.type);
			assert_equals(&subdir, each.ud);
		} assert_or {
			assert_equals("fsl_dir/file1.txt", each.path);
			assert_equals(et::created, each.type);
			assert_equals(&file1, each.ud);
		} assert_or {
			assert_equals("fsl_dir/subdir/file2.txt", each.path);
			assert_equals(et::created, each.type);
			assert_equals(&file2, each.ud);
		} assert_or {
			assert_equals("fsl_dir/subdir/file2.txt", each.path);
			assert_equals(et::created_in, each.type);
			assert_equals(&subdir, each.ud);
		});
	}
	
	}
	{
	auto changes = fsl.fetch();
	assert_equals(3u, changes.size());
	for (const auto & each : changes) {
		assert_any({
			assert_equals("fsl_dir/subdir/file2.txt", each.path);
			assert_equals(&file2, each.ud);
		} assert_or {
			assert_equals("fsl_dir/subdir", each.path);
			assert_equals(&subdir, each.ud);
		} assert_or {
			assert_equals("fsl_dir/file1.txt", each.path);
			assert_equals(&file1, each.ud);
		});
		assert_equals(fs_listener::event_t::removed, each.type);
	}
	}
}
