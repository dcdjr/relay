#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <algorithm>
#include <cerrno>

#include "client.hpp"
#include "common.hpp"

int main() {

    int clientSocket = connect_to_server();
    if (clientSocket < 0) {
        return 1;
    }

    std::string message;

    while (true) {
        fd_set readSet;
        FD_ZERO(&readSet);

        FD_SET(STDIN_FILENO, &readSet);
        FD_SET(clientSocket, &readSet);

        int maxfd = std::max(STDIN_FILENO, clientSocket);

        int ready = select(
            maxfd + 1,
            &readSet,
            nullptr,
            nullptr,
            nullptr
        );

        if (ready < 0) {
            if (errno == EINTR) {
                continue;
            }

            std::cerr << "select failed.\n";
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &readSet)) {
            if (!std::getline(std::cin, message)) break;

            if (message == "/quit") {
                break;
            }

            uint32_t message_length = message.length();

            if (message_length > MAX_MESSAGE_SIZE) {
                std::cerr << "Message exceeds maximum length of 4096 bytes.\n";
            } else if (message_length == 0) {
                std::cerr << "Message cannot be empty.\n";
            } else {
                int32_t sendMessageResult = relay::send_message(clientSocket, message.c_str(), message_length);
                if (sendMessageResult <= 0) {
                    std::cerr << "Failed to send message.\n";
                    break;
                }
            }
        }

        if (FD_ISSET(clientSocket, &readSet)) {
            char buffer[MAX_MESSAGE_SIZE + 1] = {0};

            int32_t result = relay::recv_message(
                clientSocket,
                buffer,
                sizeof(buffer)
            );

            if (result > 0) {
                std::cout << buffer << "\n";
            } else if (result == 0) {
                std::cout << "Server disconnected.\n";
                break;
            } else {
                std::cerr << "Failed to receive message.\n";
                break;
            }
        }
    }

    close(clientSocket);

    return 0;
}
