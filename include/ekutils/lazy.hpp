#ifndef LAZY_HEAD_FEDWDBYK
#define LAZY_HEAD_FEDWDBYK

#include <functional>
#include <optional>
#include <atomic>
#include <mutex>

#include <ekutils/primitives.hpp>

namespace ekutils {

template <typename T>
class lazy {
	const std::function<T(void)> factory;
	mutable std::optional<T> value;
	mutable std::mutex mutex;

public:
	explicit lazy(const std::function<T(void)> & init) : factory(init) {}
	lazy() : factory([]() -> T { return T(); }) {}

	T & get() {
		if (value.has_value())
			return *value;
		std::lock_guard lock(mutex);
		if (value.has_value())
			return *value;
		return *(value = factory());
	}

	operator T &() { return get(); }

	const T & get() const {
		if (value.has_value())
			return *value;
		std::lock_guard lock(mutex);
		if (value.has_value())
			return *value;
		return *(value = factory());
	}

	operator const T &() const { return get(); }
};

} // namespace ekutils

#endif // LAZY_HEAD_FEDWDBYK
