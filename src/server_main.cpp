#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <vector>
#include <cerrno>
#include <cstring>
#include <algorithm>

#include "common.hpp"
#include "server.hpp"

static bool in_disconnected_clients(
    int client,
    const std::vector<int>& disconnectedClients
) {
    return std::find(
        disconnectedClients.begin(),
        disconnectedClients.end(),
        client
    ) != disconnectedClients.end();
}

int main() {
    /* Set up the server's socket and get its file descriptor */
    int serverSocket = set_up_server_socket();
    if (serverSocket < 0) {
        return 1;
    }

    std::vector<int> clients;

    while (true) {
        fd_set readSet;
        FD_ZERO(&readSet);

        FD_SET(serverSocket, &readSet);
        int maxFd = serverSocket;

        for (int client : clients) {
            FD_SET(client, &readSet);

            if (client > maxFd) {
                maxFd = client;
            }
        }

        int ready = select(maxFd + 1, &readSet, nullptr, nullptr, nullptr);

        if (ready < 0) {
            /* select() may be interrupted by a signal, which is not a fatal server error
               in this case, we just want to try again. */
            if (errno == EINTR) {
                continue;
            }

            std::cerr << "select failed: " << std::strerror(errno) << "\n";
            break;
        }
        
        /* A new client wants to connect */
        if (FD_ISSET(serverSocket, &readSet)) {
            int newClient = accept(serverSocket, nullptr, nullptr);

            if (newClient < 0) {
                std::cerr << "Failed to accept client.\n";
            } else {
                clients.push_back(newClient);
                std::cout << "Client connected: " << newClient << "\n";
            }
        }

        std::vector<int> disconnectedClients;

        for (int client : clients) {
            if (!FD_ISSET(client, &readSet)) {
                continue;
            }

            /* This existing client sent data or disconnected */
            char buffer[MAX_MESSAGE_SIZE + 1] = {0};

            int32_t recvResult = relay::recv_message(
                client,
                buffer,
                sizeof(buffer)
            );

            if (recvResult > 0) {
                std::cout << "client[" << client << "]> " << buffer << "\n";
                // Broadcast message to other clients
                for (int recipient : clients) {
                    if (recipient == client) continue;
                    int broadcast_result = relay::send_message(
                        recipient,
                        buffer,
                        static_cast<uint32_t>(recvResult)
                    );

                    if (broadcast_result < 0) {
                        std::cerr
                            << "Failed to broadcast message to client: "
                            << recipient
                            << "\n";

                        if (!in_disconnected_clients(recipient, disconnectedClients)) {
                            disconnectedClients.push_back(recipient);
                        }
                    } else if (broadcast_result == 0) {
                        std::cout << "Client " << client << " disconnected.\n";
                    }
                }
            } else if (recvResult == 0) {
                std::cout << "Client disconnected: " << client << "\n";
                if (!in_disconnected_clients(client, disconnectedClients)) {
                    disconnectedClients.push_back(client);
                }
            } else {
                std::cerr << "Received error from client: " << client << "\n";
                if (!in_disconnected_clients(client, disconnectedClients)) {
                    disconnectedClients.push_back(client);
                }
            }
        }

        for (int client : disconnectedClients) {
            close(client);

            clients.erase(
                /* Moves all elements not equal to client toward the front */
                std::remove(clients.begin(), clients.end(), client),
                /* Actually deletes the unwanted leftover elements */
                clients.end()
            );
        }
    }

    for (int client : clients) {
        close(client);
    }
    
    ::close(serverSocket);

    return 0;
}
