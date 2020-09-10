#ifndef _IDGEN_HEAD
#define _IDGEN_HEAD

#include <atomic>

namespace ekutils {

template <typename ID>
class idgen {
public:
	typedef ID id_type;
private:
	std::atomic<id_type> i = id_type(0);
public:
	id_type next() {
		return i++;
	}
};

} // namespace ekutils

#endif // _IDGEN_HEAD
