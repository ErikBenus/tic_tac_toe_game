#ifndef SERVER_H
#define SERVER_H

#include "shm.h"
#include "game_logic.h"
#include "shared_names.h"
#include "socket_manager.h"
#include "shared_data.h"

typedef struct ClientArgs{
    int client_fd;
    SharedNames* names;
} ClientArgs;

typedef struct ThreadArgs{
    SharedNames* names;
    SyncData* sync_data;
} ThreadArgs;
 
void start_server(SharedNames* names, SyncData* sync_data);
void add_client(SocketManager* server, int num_players, SharedNames* names);
void* handle_client(void* arg);
void handle_player_move(int client_fd, SharedNames* shared);
void process_client_moves(SharedNames* names);
void* client_thread_function(void* args);
#endif 