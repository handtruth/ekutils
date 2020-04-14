#ifndef LAZY_HEAD_FEDWDBYK
#define LAZY_HEAD_FEDWDBYK

#include <functional>
#include <new>
#include <atomic>
#include <mutex>

#include <ekutils/primitives.hpp>

namespace ekutils {

template <typename T>
class lazy {
    const std::function<T(void)> factory;
    mutable byte_t data[sizeof(T)];
    mutable volatile bool initialized = false;
    mutable std::mutex mutex;

    constexpr T & get_data() {
        return *reinterpret_cast<T *>(data);
    }

    constexpr const T & get_data() const {
        return *reinterpret_cast<const T *>(data);
    }

public:
    explicit lazy(const std::function<T(void)> & init) : factory(init) {}
    lazy() : factory([]() -> T { return T(); }) {}

    T & get() {
        if (initialized)
            return get_data();
        std::lock_guard lock(mutex);
        if (initialized)
            return get_data();
        new(data) T(factory());
        initialized = true;
        return get_data();
    }

    operator T &() { return get(); }

    const T & get() const {
        if (initialized)
            return get_data();
        std::lock_guard lock(mutex);
        if (initialized)
            return get_data();
        new(data) T(factory());
        initialized = true;
        return get_data();
    }

    operator const T &() const { return get(); }

    ~lazy() {
        if (initialized)
            get_data().~T();
    }
};

} // namespace ekutils

#endif // LAZY_HEAD_FEDWDBYK
