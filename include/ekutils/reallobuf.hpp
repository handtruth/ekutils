#ifndef _REALLOBUF_HEAD
#define _REALLOBUF_HEAD

#include "ekutils/primitives.hpp"

namespace ekutils {

class reallobuf final {
private:
    std::size_t sz;
    byte_t * bytes;
public:
    const std::size_t k = 1;
    explicit reallobuf(std::size_t initial = 8);
    reallobuf(const reallobuf & other);
    reallobuf(reallobuf && other);
    std::size_t probe(std::size_t amount);
    void move(std::size_t i);
    std::size_t size() const noexcept {
        return sz;
    }
    byte_t * data() noexcept {
        return bytes;
    }
    const byte_t * data() const noexcept {
        return bytes;
    }
    ~reallobuf();
};

} // ekutils

#endif // _REALLOBUF_HEAD
