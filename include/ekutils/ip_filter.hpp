#ifndef IP_FILTER_HEAD_APPVKRJSJVJBF
#define IP_FILTER_HEAD_APPVKRJSJVJBF

#include <ekutils/ip_space.hpp>

namespace ekutils::net {

struct ip_filter final {
	enum class modes {
		any, denylist, allowlist, nobody
	} mode = modes::any;
	addresses data;

	bool is_allowed(const endpoint & target) const {
		switch (mode) {
		case modes::any:
			return true;
		case modes::denylist:
			return !data.contains(target);
		case modes::allowlist:
			return data.contains(target);
		case modes::nobody:
			return false;
		default:
			throw std::domain_error("unknown mode");
		}
	}

	bool allow(const std::string_view & str) {
		switch (mode) {
		case modes::any:
		case modes::nobody:
			mode = modes::allowlist;
			[[fallthrough]];
		case modes::allowlist:
			return data.add(str);
		case modes::denylist:
			throw std::domain_error("address filter operates in denylist mode");
		default:
			throw std::domain_error("unknown mode");
		}
	}

	bool deny(const std::string_view & str) {
		switch (mode) {
		case modes::any:
		case modes::nobody:
			mode = modes::denylist;
			[[fallthrough]];
		case modes::denylist:
			return data.add(str);
		case modes::allowlist:
			throw std::domain_error("address filter operates in allowlist mode");
		default:
			throw std::domain_error("unknown mode");
		}
	}
};

} // namespace ekutils::net

#endif // IP_FILTER_HEAD_APPVKRJSJVJBF
