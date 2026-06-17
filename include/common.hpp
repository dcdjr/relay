#pragma once

#include <sys/socket.h>
#include <cstddef>

namespace relay {
    std::size_t send_all(int collector_fd, const char* buffer, std::size_t length);
    bool recv_exact(int agent_fd, void* buffer, std::size_t length);
}
