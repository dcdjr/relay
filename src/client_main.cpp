#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>

#include "client.hpp"

int main() {

    int clientSocket = connect_to_server();

    std::string message;

    while (true) {
        std::getline(std::cin, message);

        if (message == "/quit") {
            break;
        }

        send(clientSocket, message.c_str(), message.length(), 0);
    }

    close(clientSocket);

    return 0;
}
