#ifndef _EVENT_D_HEAD
#define _EVENT_D_HEAD

#include "ekutils/descriptor.hpp"

namespace ekutils {

class event_d : public descriptor {
public:
	explicit event_d(unsigned int initval = 0);
	std::uint64_t read();
	void write(std::uint64_t value);
	virtual std::string to_string() const noexcept override;
};

} // namespace ekutils

#endif // _EVENT_D_HEAD
