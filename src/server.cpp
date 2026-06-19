#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
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
    if (setsockopt(
        serverSocket,
        SOL_SOCKET,
        SO_REUSEADDR,
        &opt,
        sizeof(opt)) == -1
    ) {
        std::cerr << "Could not set socket options\n";
        close(serverSocket);
        return -1;
    }

    /* Struct to hold the server's information
       htons() converts port to network byte order
       INADDR_ANY means to accept connections on any IP */
    sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    /* Bind the socket to an address */
    int bindResult = bind(
        serverSocket,
        (struct sockaddr*)&serverAddress,
        sizeof(serverAddress)
    );
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
