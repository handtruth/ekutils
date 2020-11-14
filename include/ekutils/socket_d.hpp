#ifndef SOCKET_D_HEAD_UJTGRFRCGTHJU
#define SOCKET_D_HEAD_UJTGRFRCGTHJU

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>

enum class family_t {
    ipv4 = AF_INET,
    ipv6 = AF_INET6,
    un = AF_UNIX,
};

struct endpoint {
    virtual family_t family() const noexcept = 0;
    virtual ~endpoint() {}
};

struct udp_endpoint {
    virtual family_t family() const noexcept {
        return 
    }
};

#endif // SOCKET_D_HEAD_UJTGRFRCGTHJU
