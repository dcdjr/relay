#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

#include "server.hpp"

#define BUF_SIZE 1024

int main() {

    int serverSocket = set_up_server_socket();
    if (serverSocket < 0) {
        std::cerr << "Failed to set up server socket.\n";
        return 1;
    }

    int clientSocket = accept(serverSocket, nullptr, nullptr);

    std::cout << "Client connected." << std::endl;

    while (true) {
        char buffer[BUF_SIZE] = {0};
        int recvResult = recv(clientSocket, buffer, sizeof(buffer), 0);
        
        if (recvResult > 0) {
            std::cout << "client> " << buffer << std::endl;
        } else if (recvResult == 0) {
            std::cout << "Client disconnected." << std::endl;
            break;
        } else {
            std::cerr << "There was an error receiving the message from the client." << std::endl;
            break;
        }

    }
    
    ::close(serverSocket);
    ::close(clientSocket);
    
    return 0;
}
