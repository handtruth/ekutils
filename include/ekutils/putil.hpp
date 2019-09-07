#ifndef _PUTIL_HEAD
#define _PUTIL_HEAD

namespace ekutils {

template <typename F>
class finalizator {
	F block;
public:
	finalizator(const finalizator & other) = delete;
	finalizator(finalizator && other) = delete;
	explicit constexpr finalizator (F lambda) : block(lambda) {}
	~finalizator() {
		block();
	}
};

#define __ekutils_finnaly_internal(line, block) \
		finalizator __fblock##line([&]() -> void block )
#define finnaly(block) \
		__ekutils_finnaly_internal(__LINE__, block)

} // ekutils

#endif // _PUTIL_HEAD
