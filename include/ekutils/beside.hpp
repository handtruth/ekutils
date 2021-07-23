#ifndef BESIDE_HEAD_FRVLMWDMKPK
#define BESIDE_HEAD_FRVLMWDMKPK

namespace handtruth::ekutils {

template <typename T>
struct beside {
	bool operator()(const T & a, const T & b) const {
		return a + 1 == b;
	}
};

} // namespace handtruth::ekutils

#endif // BESIDE_HEAD_FRVLMWDMKPK
