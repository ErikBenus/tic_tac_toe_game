#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "server.h"
#include "socket_manager.h"

void start_server(char serverName[30]) {
    SharedNames names;
    createNames(serverName, &names);
    shm_init(&names);
    printf("podarilo še \n");
    add_client();
    }

void add_client() {
    SocketManager* server = create_socket(8080, 1);
    if (!server) {
        fprintf(stderr, "Failed to create server socket\n");
        exit(EXIT_FAILURE);
    }

    int current_player = 0;
    while (current_player < 3) {
        int client_fd;
        socklen_t addr_len = sizeof(server->address);
        client_fd = accept(server->socket_fd, (struct sockaddr*)&server->address, &addr_len);
        if (client_fd < 0) {
            perror("Accept failed");
        }

        // Prijímanie mena
        char player_name[30];
        memset(player_name, 0, sizeof(player_name));
        if (receive_data(client_fd, player_name, sizeof(player_name)) <= 0) {
            perror("Failed to receive player name");
            close(client_fd);
        }

        // Prijímanie symbolu
        char player_symbol;
        if (receive_data(client_fd, &player_symbol, sizeof(player_symbol)) <= 0) {
            perror("Failed to receive player symbol");
            close(client_fd);
        }

        printf("Received player name: %s\n", player_name);
        printf("Received player symbol: %c\n", player_symbol);

    
        send_data(client_fd, &current_player, sizeof(current_player));
        current_player++;
        close(client_fd);
    }

    close_socket(server);
}