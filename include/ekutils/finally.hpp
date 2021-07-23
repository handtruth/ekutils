#ifndef _PUTIL_HEAD
#define _PUTIL_HEAD

namespace handtruth::ekutils {

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

#define __ekutils_finally_internal(line, block) \
		::handtruth::ekutils::finalizator __fblock##line([&]() -> void { block })
#define finally(block) \
		__ekutils_finally_internal(__LINE__, block)

} // handtruth::ekutils

#endif // _PUTIL_HEAD
