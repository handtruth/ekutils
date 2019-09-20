#include "ekutils/expandbuff.hpp"

#include <cstdlib>
#include <cstring>
#include <cassert>

#include <algorithm>

namespace ekutils {

expandbuff::expandbuff(std::size_t size) : sz(0), allocated(size) {
	if (size)
		bytes = reinterpret_cast<byte_t *>(std::malloc(size));
	else
		bytes = nullptr;
}

void expandbuff::check(std::size_t size) {
	std::size_t new_cap = std::max(allocated, std::size_t(10));
	while (new_cap < size) {
		new_cap *= 2;
	}
	if (new_cap != allocated)
		bytes = reinterpret_cast<byte_t *>(std::realloc(bytes, allocated = new_cap));
}

void expandbuff::size(std::size_t size) {
	check(size);
	sz = size;
}

void expandbuff::append(const byte_t * data, std::size_t size) {
	check(sz + size);
	std::memcpy(bytes + sz, data, size);
	sz += size;
}

void expandbuff::move(std::size_t k) {
	assert(k <= sz);
	std::memmove(bytes, bytes + k, sz - k);
	sz -= k;
}

void expandbuff::shrink_to_fit() {
	if (allocated != sz)
		bytes = reinterpret_cast<byte_t *>(std::realloc(bytes, sz));
}

expandbuff::~expandbuff() {
	std::free(bytes);
}

} // namespace ekutils
