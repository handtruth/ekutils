#ifndef CPPWRAPS_ACTION_HEAD_OVOJRJIKUSI
#define CPPWRAPS_ACTION_HEAD_OVOJRJIKUSI

#include <memory>

namespace handtruth::ekutils {

template <typename F>
class callable_base;

template <typename R, typename ...Args>
class callable_base<R(Args...)> {
public:
	virtual R operator()(Args... args) = 0;
	virtual ~callable_base() {}
};

template <typename C, typename F>
class callable_virt;

template <typename C, typename R, typename ...Args>
class callable_virt<C, R(Args...)> : public callable_base<R(Args...)> {
	C _callable;
public:
	callable_virt(C callable) : _callable(callable) {}
	virtual R operator()(Args... args) override {
		return _callable(args...);
	}
};

template <typename F>
class delegate;

template <typename R, typename ...Args>
class delegate<R(Args...)> {
	std::unique_ptr<callable_base<R(Args...)>> _callable;

public:
	delegate() = default;
	template <typename C>
	delegate(C callable) : _callable(std::make_unique<callable_virt<C, R(Args...)>>(callable)) {}

	R operator()(Args... args) {
		return (*_callable)(args...);
	}
};

} // namespace handtruth::ekutils

#endif // CPPWRAPS_ACTION_HEAD_OVOJRJIKUSI
