#ifndef LATEINIT_HEAD_GNNFBDFT
#define LATEINIT_HEAD_GNNFBDFT

#include <optional>
#include <stdexcept>

namespace ekutils {

template <typename T>
class lateinit {
	std::optional<T> value;

	void check_init() {
		if (value.has_value())
			throw std::runtime_error("property already initialized");
	}

public:
	bool initialized() const {
		return value.has_value();
	}

	template <typename ...Args>
	T & emplace(Args &&... args) {
		check_init();
		return value.emplace(std::forward<Args>(args)...);
	}

	lateinit & operator=(const T & data) {
		check_init();
		value = data;
		return *this;
	}

	lateinit & operator=(T && data) {
		check_init();
		value = std::move(data);
		return *this;
	}

	const T & get() const {
		return value.value();
	}

	operator const T &() const {
		return get();
	}

	T & get() {
		return value.value();
	}
	
	operator T &() {
		return get();
	}
};

} // namespace ekutils

#endif // LATEINIT_HEAD_GNNFBDFT
