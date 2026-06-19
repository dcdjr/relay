#include <netinet/in.h>
#include <iostream>

#include "common.hpp"

int32_t relay::send_all(int socket_fd, const char* buffer, uint32_t length) {
    uint32_t total_sent = 0; // Bytes sent so far
    uint32_t bytes_remaining = length; // Bytes left to send
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

int32_t relay::recv_exact(int socket_fd, void* buffer, uint32_t length) {
    // Cast to char pointer so the buffer is offset by `received` bytes
    int32_t total_received = 0;
    char* ptr = static_cast<char*>(buffer);
    while (length > 0) {
        ssize_t received = recv(socket_fd, ptr, length, 0);

        if (received < 0) {
            return -1;
        } else if (received == 0) {
            return 0;
        }

        ptr += received; // Move buffer pointer forward
        length -= received; // Decrease remaining bytes to read
        total_received += received;
    }

    return total_received;
}

int32_t relay::send_message(int socket_fd, const char* buffer, uint32_t length) {
    uint32_t network_order_length = htonl(length);

    int32_t length_n = relay::send_all(
        socket_fd,
        reinterpret_cast<const char*>(&network_order_length),
        sizeof(network_order_length)
    );
    if (length_n < 0) {
        std::cerr << "Failed to send length header.\n";
        return -1;
    } else if (length_n == 0) {
        return 0;
    }

    int32_t message_n = relay::send_all(socket_fd, buffer, length);
    if (message_n < 0) {
        std::cerr << "Failed to send length header.\n";
        return -1;
    } else if (message_n == 0) {
        return 0;
    }

    return message_n;
}

int32_t relay::recv_message(int socket_fd, char* buffer, uint32_t capacity) {
    uint32_t network_order_length{};
    int32_t recvLengthResult = relay::recv_exact(socket_fd, &network_order_length, sizeof(network_order_length));
    if (recvLengthResult < 0) {
        std::cerr << "Failed to receive message length.\n";
        return -1;
    } else if (recvLengthResult == 0) {
        return 0;
    }
    
    uint32_t host_order_length = ntohl(network_order_length);

    if (host_order_length >= capacity) {
        std::cerr << "Message exceeded size limit of 4096 bytes.\n";
        return -1;
    }
    
    int32_t recvMessageContentResult = relay::recv_exact(socket_fd, buffer, host_order_length);
    if (recvMessageContentResult < 0) {
        std::cerr << "Failed to receive message content.\n";
        return -1;
    } else if (recvMessageContentResult == 0) {
        return 0;
    }

    buffer[host_order_length] = '\0';

    return recvMessageContentResult;
}
