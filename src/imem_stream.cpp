#include "ekutils/imem_stream.hpp"

#include <cstring>
#include <iostream>

namespace ekutils {

int imem_stream::read(byte_t data[], std::size_t length) {
	if (fetched == size)
		return -1;
	std::size_t reminds = size - fetched;
	if (length > reminds) {
		std::memcpy(data, blob + fetched, reminds);
		fetched = size;
		return static_cast<int>(reminds);
	} else {
		std::memcpy(data, blob + fetched, length);
		fetched += length;
		return static_cast<int>(length);
	}
}

} // namespace ekutils
