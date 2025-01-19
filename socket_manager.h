#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "shared_names.h"

typedef struct {
    int socket_fd;
    struct sockaddr_in address;
} SocketManager;

SocketManager* create_socket(int port, int is_server);
void close_socket(SocketManager* manager);
int send_data(int socket_fd, const void* data, size_t size);
int receive_data(int socket_fd, void* buffer, size_t size);
int hashString(const char *str);

#endif
