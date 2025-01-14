#ifndef SHM_H
#define SHM_H

#include "game_logic.h"
#include "shared_names.h"

// typedef struct Server {
//     GameLogic *game;       // Ukazovateľ na hernú logiku
//     void *shm_ptr;         // Ukazovateľ na zdieľanú pamäť
//     int max_clients;       // Maximálny počet klientov
//     int connected_clients; // Počet pripojených klientov
//     int shm_fd_;
// } Server;


void shm_init(SharedNames names);
void shm_destroy(SharedNames names);
void server_shm_open(SharedNames names, int *out_fd_shm);
void server_shm_close(SharedNames names);

#endif