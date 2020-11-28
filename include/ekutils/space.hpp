#ifndef SPACE_HEAD_FVEFCPGDMC
#define SPACE_HEAD_FVEFCPGDMC

#include <map>

#include <ekutils/beside.hpp>

namespace ekutils {

template <
	typename T,
	typename B = ekutils::beside<T>,
	typename comparator = std::less<T>,
	typename allocator = std::allocator<std::pair<const T, T>>
>
struct space {
	typedef T value_type;
	typedef B beside_t;
	typedef std::map<T, T, comparator, allocator> container_t;
	typedef typename container_t::iterator iterator;
	typedef typename container_t::const_iterator const_iterator;
private:
	beside_t m_beside;
	container_t m_ranges;

public:
	const container_t & ranges() const noexcept {
		return m_ranges;
	}

	beside_t beside() const {
		return m_beside;
	}

	void below(iterator & it) {
		if (it != m_ranges.begin())
			--it;
		else
			it = m_ranges.end();
	}

	void above(iterator & it) {
		if (it != m_ranges.end())
			++it;
	}

private:
	std::pair<iterator, bool> private_insert(const value_type & item) {
		auto end = m_ranges.end();
		auto upper = m_ranges.upper_bound(item);
		auto lower = upper;
		below(lower);

		if (lower == end) {
			// smollest
			if (upper == end) {
				// space is empty, create first range
				return m_ranges.emplace(item, item);
			} else {
				if (m_beside(item, upper->first)) {
					const_cast<value_type &>(upper->first) = item; // should not be required to rebalance tree
					return std::make_pair(upper, true);
				} else {
					// create new range
					return m_ranges.emplace(item, item);
				}
			}
		} else {
			// not smollest
			if (m_ranges.key_comp()(lower->second, item)) {
				// not in existing range
				if (upper == end) {
					// last range
					if (m_beside(lower->second, item)) {
						// modify existing range
						lower->second = item;
						return std::make_pair(lower, true);
					} else {
						// create new last range
						return m_ranges.emplace(item, item);
					}
				} else {
					// between existing ranges
					if (m_beside(lower->second, item)) {
						if (m_beside(item, upper->first)) {
							// fills gap between 2 ranges
							// remove upper and modify lower
							lower->second = std::move(upper->second);
							m_ranges.erase(upper);
							return std::make_pair(lower, true);
						} else {
							// attach to lower range
							lower->second = item;
							return std::make_pair(lower, true);
						}
					} else {
						if (m_beside(item, upper->first)) {
							const_cast<value_type &>(upper->first) = item; // should not be required to rebalance tree
							return std::make_pair(upper, true);
						} else {
							// insert new range between ranges
							return m_ranges.emplace(item, item);
						}
					}
				}
			} else {
				// already in space
				return std::make_pair(lower, false);
			}
		}
	}
	
	std::pair<iterator, bool> private_insert(const value_type & a, const value_type & b) {
		if (m_ranges.key_comp()(b, a))
			return private_insert(b, a);
		auto lower = private_insert(a);
		auto upper = private_insert(b);
		if (lower.first == upper.first)
			return std::make_pair(lower.first, lower.second || upper.second);
		lower.first->second = std::move(upper.first->second);
		auto start = lower.first;
		above(start);
		auto end = upper.first;
		above(end);
		m_ranges.erase(start, end);
		return std::make_pair(lower.first, true);
	}

public:
	std::pair<const_iterator, bool> insert(const value_type & item) {
		return private_insert(item);
	}

	std::pair<const_iterator, bool> insert(const value_type & a, const value_type & b) {
		return private_insert(a, b);
	}

	bool add(const value_type & item) {
		return insert(item).second;
	}

	bool add(const value_type & a, const value_type & b) {
		return insert(a, b).second;
	}

	bool contains(const value_type & it) const {
		auto lower = m_ranges.upper_bound(it);
		if (lower != m_ranges.begin())
			--lower;
		else
			return false;
		return !m_ranges.key_comp()(lower->second, it);
	}

	bool empty() const {
		return m_ranges.empty();
	}
};

} // namespace ekutils

#endif // SPACE_HEAD_FVEFCPGDMC
