#ifndef _UUID_HEAD
#define _UUID_HEAD

#include <cinttypes>
#include <string>

#include <ekutils/primitives.hpp>

namespace handtruth::ekutils {

struct uuid {
	union {
		byte_t bytes[16];
		struct {
			std::uint64_t most;
			std::uint64_t least;
		} data;
	};
	constexpr uuid(std::uint64_t most_bits = 0, std::uint64_t least_bits = 0) :
		data { most_bits, least_bits } {}
	static uuid random() noexcept;
	operator std::string() const;
};

} // namespace handtruth::ekutils

#endif // _UUID_HEAD
