#ifndef SERVER_H
#define SERVER_H

#include "shm.h"
#include "game_logic.h"
#include "shared_names.h"
#include "socket_manager.h"

typedef struct {
    SharedNames* names;
} ThreadArgs;

void start_server(SharedNames* names);
void add_client(SocketManager* server, int num_players, SharedNames* names);
void* handle_client(void* arg);
//void process_move(SocketManager* server, Player* player);
void handle_player_move(int client_fd, SharedNames* shared);
void process_client_moves(SharedNames* names);
void* client_thread_function(void* args);
#endif 