#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 5150

int connect_to_server() {
    /* Set up client TCP socket using the IPv4 protocol */
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Failed to initialize client socket.\n";
        return -1;
    }

    struct sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    /* Connect to server */
    int connectResult = connect(
        clientSocket,
        (struct sockaddr*)&serverAddress,
        sizeof(serverAddress)
    );
    if (connectResult < 0) {
        std::cerr << "Client failed to connect to server.\n";
        close(clientSocket);
        return 1;
    }

    return clientSocket;
}



