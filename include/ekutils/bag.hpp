#ifndef BAG_HEAD_GIGCFTHCWDZCXVCB
#define BAG_HEAD_GIGCFTHCWDZCXVCB

#include <forward_list>

namespace handtruth::ekutils {

template <typename T>
class bag {
	std::forward_list<T> data;

public:
	T & add(const T & value) {
		data.push_front(value);
		return data.front();
	}
	
	T & add(T && value) {
		data.push_front(std::move(value));
		return data.front();
	}

	template <typename ...Args>
	T & emplace(Args &&... args) {
		return data.emplace_front(std::forward<Args>(args)...);
	}

	void clear() {
		data.clear();
	}
};

} // namespace handtruth::ekutils

#endif // BAG_HEAD_GIGCFTHCWDZCXVCB
