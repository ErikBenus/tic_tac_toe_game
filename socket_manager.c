#include "socket_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

SocketManager* create_socket(int port, int is_server) {
    SocketManager* manager = malloc(sizeof(SocketManager));
    if (!manager) {
        perror("Failed to allocate memory for SocketManager");
        return NULL;
    }

    manager->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (manager->socket_fd < 0) {
        perror("Socket creation failed");
        free(manager);
        return NULL;
    }

    manager->address.sin_family = AF_INET;
    manager->address.sin_port = htons(port);
    manager->address.sin_addr.s_addr = is_server ? INADDR_ANY : inet_addr("127.0.0.1");

    if (is_server) {
        if (bind(manager->socket_fd, (struct sockaddr*)&manager->address, sizeof(manager->address)) < 0) {
            perror("Bind failed");
            close(manager->socket_fd);
            free(manager);
            return NULL;
        }

        if (listen(manager->socket_fd, MAX_PLAYERS) < 0) {
            perror("Listen failed");
            close(manager->socket_fd);
            free(manager);
            return NULL;
        }
    }
    return manager;
}

void close_socket(SocketManager* manager) {
    if (manager) {
        close(manager->socket_fd);
        free(manager);
    }
}

int send_data(int socket_fd, const void* data, size_t size) {
    return send(socket_fd, data, size, 0);
}

int receive_data(int socket_fd, void* buffer, size_t size) {
    return recv(socket_fd, buffer, size, 0);
}

int hashString(const char *str) {
    unsigned long hash = 0; 
    int c;
    while ((c = *str++)) {
        hash = hash * 31 + c; 
    }
    return (int)(hash % 2147483647); 
}


