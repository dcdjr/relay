#include "common.hpp"

std::size_t relay::send_all(int socket_fd, const char* buffer, std::size_t length) {
    std::size_t total_sent = 0; // Bytes sent so far
    std::size_t bytes_remaining = length; // Bytes left to send
    ssize_t n;

    while (total_sent < length) {
        // Offset the buffer pointer by the number of bytes already sent
        n = send(socket_fd, buffer + total_sent, bytes_remaining, 0);

        if (n <= 0) return -1;

        total_sent += n;
        bytes_remaining -= n;
    }

    return total_sent;
}

bool relay::recv_exact(int socket_fd, void* buffer, std::size_t length) {
    // Cast to char pointer so the buffer is offset by `received` bytes
    char* ptr = static_cast<char*>(buffer);
    while (length > 0) {
        ssize_t received = recv(socket_fd, ptr, length, 0);

        if (received < 0) {
            return false;
        } else if (received == 0) {
            return false;
        }

        ptr += received; // Move buffer pointer forward
        length -= received; // Decrease remaining bytes to read
    }

    return true;
}
