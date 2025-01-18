#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

typedef struct {
    int socket_fd;
    struct sockaddr_in address;
} SocketManager;

typedef struct {
    int* client_sockets;  // Pole socketov klientov
    int num_clients;      // Počet klientov
    pthread_mutex_t lock; // Na ochranu pri pridávaní/odstraňovaní klientov
} ClientManager;

SocketManager* create_socket(int port, int is_server);
void close_socket(SocketManager* manager);
int send_data(int socket_fd, const void* data, size_t size);
int receive_data(int socket_fd, void* buffer, size_t size);
int hashString(const char *str);

// Inicializácia
ClientManager* create_client_manager(int max_clients);

// Pridanie klienta
void add_client_socket(ClientManager* manager, int client_socket);

// Odstránenie klienta (napr. po odpojení)
void remove_client(ClientManager* manager, int client_socket);


#endif
