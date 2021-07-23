#ifndef POCKET_HEAD_WCBDEWACCDVPO
#define POCKET_HEAD_WCBDEWACCDVPO

#include <stdexcept>

namespace handtruth::ekutils {

template <typename T>
class pocket {
	T * resource = nullptr;
public:
	pocket() = default;
	class guard {
		pocket & m_pocket;

		guard(pocket & p, T * resource) : m_pocket(p) {
			if (p.resource)
				throw std::runtime_error("resource is being used");
			p.resource = resource;
		}
	public:
		~guard() {
			m_pocket.resource = nullptr;
		}
		friend pocket;
	};
	operator T &() const {
		if (resource)
			return *resource;
		else
			throw std::runtime_error("resource not created");
	}
	T & get() const {
		return *this;
	}
	guard use(T & resource) {
		return guard(*this, &resource);
	}
};

} // namespace handtruth::ekutils

#endif // POCKET_HEAD_WCBDEWACCDVPO
