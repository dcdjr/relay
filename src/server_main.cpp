#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

#include "common.hpp"
#include "server.hpp"

int main() {
    /* Set up the server's socket and get its file descriptor */
    int serverSocket = set_up_server_socket();
    if (serverSocket < 0) {
        return 1;
    }

    int clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket < 0) {
        std::cerr << "Failed to accept client.\n";
        close(serverSocket);
        return 1;
    }

    std::cout << "Client connected." << std::endl;

    while (true) {
        char buffer[MAX_MESSAGE_SIZE + 1] = {0};
        int32_t recvResult = relay::recv_message(clientSocket, buffer, sizeof(buffer));
        
        if (recvResult < 0) {
            std::cerr << "There was an error receiving the message from the client." << std::endl;
            break;
        } else if (recvResult == 0) {
            std::cout << "Client disconnected.\n";
            break;
        } else {
            std::cout << "client> " << buffer << "\n";
        }
    }
    
    ::close(serverSocket);
    ::close(clientSocket);
    
    return 0;
}
