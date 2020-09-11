#include "ekutils/timer_d.hpp"

#include <sys/timerfd.h>
#include <ctime>
#include <unistd.h>
#include <system_error>
#include <cerrno>
#include <limits>

namespace ekutils {

timer_d::timer_d() {
	handle = timerfd_create(CLOCK_MONOTONIC, 0);
	if (handle == -1)
		throw std::system_error(std::make_error_code(std::errc(errno)), "failed to create timerfd");
}

void timer_d::delay(unsigned long secs, unsigned long nanos) {
	timespec now;
	if (clock_gettime(CLOCK_MONOTONIC, &now) == -1)
		throw std::system_error(std::make_error_code(std::errc(errno)), "error while setting timer delay");
	itimerspec new_value;
	new_value.it_interval.tv_sec = 0;
	new_value.it_interval.tv_nsec = 0;
	new_value.it_value.tv_sec = now.tv_sec + static_cast<time_t>(secs);
	new_value.it_value.tv_nsec = now.tv_nsec + static_cast<long>(nanos);
	if (timerfd_settime(handle, 0, &new_value, NULL) == -1)
		throw std::system_error(std::make_error_code(std::errc(errno)), "error while setting timer delay");
}

void timer_d::period(unsigned long secs, unsigned long nanos) {
	itimerspec new_value;
	auto csecs = static_cast<time_t>(secs);
	auto cnanos = static_cast<long>(nanos);
	new_value.it_interval.tv_sec = csecs;
	new_value.it_interval.tv_nsec = cnanos;
	new_value.it_value.tv_sec = csecs;
	new_value.it_value.tv_nsec = cnanos;
	if (timerfd_settime(handle, 0, &new_value, NULL) == -1)
		throw std::system_error(std::make_error_code(std::errc(errno)), "error while setting timer period");
}

std::uint64_t timer_d::read() {
	std::uint64_t result;
	int r = ::read(handle, &result, sizeof(result));
	if (r == -1) {
		if (errno == EWOULDBLOCK)
			return std::numeric_limits<std::uint64_t>::max();
		else
			throw std::system_error(std::make_error_code(std::errc(errno)), "error while reading timer status");
	}
	return result;
}

std::string timer_d::to_string() const noexcept {
	return "timer";
}

} // namespace ekutils
