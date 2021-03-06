#ifndef _TIMER_D_HEAD
#define _TIMER_D_HEAD

#include <chrono>

#include <ekutils/descriptor.hpp>

namespace ekutils {

class timer_d : public descriptor {
public:
	timer_d();
	void delay(unsigned long secs, unsigned long nanos);
	template <typename Rep, typename Period>
	void delay(std::chrono::duration<Rep, Period> span) {
		using namespace std::chrono;
		seconds span_sec = duration_cast<seconds>(span);
		auto secs = static_cast<unsigned long>(span_sec.count());
		auto nanos = static_cast<unsigned long>(duration_cast<nanoseconds>(span_sec - floor<seconds>(span_sec)).count());
		delay(secs, nanos);
	}
	void period(unsigned long secs, unsigned long nanos);
	template <typename Rep, typename Period>
	void period(std::chrono::duration<Rep, Period> span) {
		using namespace std::chrono;
		seconds span_sec = duration_cast<seconds>(span);
		auto secs = static_cast<unsigned long>(span_sec.count());
		auto nanos = static_cast<unsigned long>(duration_cast<nanoseconds>(span_sec - floor<seconds>(span_sec)).count());
		period(secs, nanos);
	}
	std::uint64_t read();
	virtual std::string to_string() const noexcept override;
};

} // namespace ekutils

#endif // _TIMER_D_HEAD
