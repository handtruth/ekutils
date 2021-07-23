#ifndef _MUTEX_ATOMIC_HEAD
#define _MUTEX_ATOMIC_HEAD

#include <shared_mutex>

namespace handtruth::ekutils {

template <typename T>
class matomic {
private:
	mutable std::shared_mutex mutex;
	T object;
public:
	matomic() {}
	matomic(const T & data) : object(data) {}
	matomic(T && data) : object(static_cast<T &&>(data)) {}
	matomic & operator=(const T & data) {
		std::unique_lock lock(mutex);
		object = data;
		return *this;
	}
	matomic & operator=(T && data) {
		std::unique_lock lock(mutex);
		object = std::move(data);
		return *this;
	}
	T get() const {
		std::shared_lock lock(mutex);
		return object;
	}
	operator T() const {
		return get();
	}
};

} // namespace handtruth::ekutils


#endif // _MUTEX_ATOMIC_HEAD
