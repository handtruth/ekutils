#ifndef BESIDE_HEAD_FRVLMWDMKPK
#define BESIDE_HEAD_FRVLMWDMKPK

namespace ekutils {

template <typename T>
struct beside {
	bool operator()(const T & a, const T & b) const {
		return a + 1 == b;
	}
};

} // namespace ekutils

#endif // BESIDE_HEAD_FRVLMWDMKPK
