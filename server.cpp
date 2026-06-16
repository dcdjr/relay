#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

#define SERVER_PORT 5150
#define BUF_SIZE 1024

int set_up_server_socket() {
    /* Set up a TCP socket using the IPv4 protcol */
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Failed to initialize server socket.\n";
        return -1;
    }

    /* Enable SO_REUSEADDR so collector OS allows the port to be reused quickly */
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* Struct to hold the server's information
       htons() converts port to network byte order
       INADDR_ANY means to accept connections on any IP */
    sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    /* Bind the socket to an address */
    int bindResult = bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if (bindResult < 0) {
        std::cerr << "Failed to bind server socket.\n";
        ::close(serverSocket);
        return -1;
    }

    /* Listen for incoming connections */
    int backLog = 10;
    int listenResult = listen(serverSocket, backLog);
    if (listenResult < 0) {
        std::cerr << "Failed to listen on server socket.\n";
        ::close(serverSocket);
        return -1;
    }

    return serverSocket;
}

int main() {

    int serverSocket = set_up_server_socket();
    if (!serverSocket) {
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
