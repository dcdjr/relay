#pragma once

#include <sys/socket.h>
#include <cstdint>

#define MAX_MESSAGE_SIZE 4096

namespace relay {
    int32_t send_all(int socket_fd, const char* buffer, uint32_t length);
    int32_t recv_exact(int socket_fd, void* buffer, uint32_t length);

    int32_t send_message(int socket_fd, const char* buffer, uint32_t length);
    int32_t recv_message(int socket_fd, char* buffer, uint32_t capacity);
}
