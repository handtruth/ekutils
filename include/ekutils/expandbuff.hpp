#ifndef _EXPANDBUFF_HEAD
#define _EXPANDBUFF_HEAD

#include <array>

#include <ekutils/primitives.hpp>

namespace handtruth::ekutils {

class expandbuff {
	byte_t * bytes;
	std::size_t sz;
	std::size_t allocated;
	void check(std::size_t size);
public:
	explicit expandbuff(std::size_t size = 0);
	constexpr byte_t * data() noexcept {
		return bytes;
	}
	constexpr const byte_t * data() const noexcept {
		return bytes;
	}
	constexpr std::size_t size() const noexcept {
		return sz;
	}
	constexpr std::size_t capacity() const noexcept {
		return allocated;
	}
	constexpr void clear() noexcept {
		sz = 0;
	}
	void size(std::size_t size);
	inline void asize(std::size_t length) {
		size(sz + length);
	}
	inline void ssize(std::size_t length) {
		size(sz - length);
	}
	void append(const byte_t * data, std::size_t size);
	template <std::size_t N>
	constexpr void append(const std::array<byte_t, N> & data, const std::size_t size = N) {
		append(data.data(), size);
	}
	void move(std::size_t k);
	void shrink_to_fit();

	~expandbuff();
};

} // namespace handtruth::ekutils

#endif // _EXPANDBUFF_HEAD
