#include "ekutils/omem_stream.hpp"

#include <cstring>

namespace ekutils {

int omem_stream::write(const byte_t data[], std::size_t length) {
	buff.append(data, length);
	return length;
}

} // namespace ekutils
