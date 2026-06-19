#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>

#include "client.hpp"
#include "common.hpp"


int main() {

    int clientSocket = connect_to_server();

    std::string message;

    while (true) {
        std::getline(std::cin, message);

        if (message == "/quit") {
            break;
        }

        if (message.empty()) {
            continue;
        }

        uint32_t message_length = message.length();

        if (message_length > MAX_MESSAGE_SIZE) {
            std::cerr << "Message exceeds maximum length of 4096 bytes.\n";
            continue;
        }

        int32_t sendMessageResult = relay::send_message(clientSocket, message.c_str(), message_length);
    }

    close(clientSocket);

    return 0;
}
