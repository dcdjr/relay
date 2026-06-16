#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 5150

int set_up_client_socket() {
    /* Set up client TCP socket using the IPv4 protocol */
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (!clientSocket) {
        std::cerr << "Failed to initialize client socket.\n";
        return -1;
    }

    return clientSocket;
}


int main() {

    int clientSocket = set_up_client_socket();

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
