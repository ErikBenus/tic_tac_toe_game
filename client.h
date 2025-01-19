#ifndef CLIENT_H
#define CLIENT_H

#include "shm.h"
#include "game_logic.h"
#include "server.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

typedef struct Player {
    int ID; //číslo, ktorý je vporadí, každý hráč bude mať unikátne od 0+
    char name[MAX_NAME_LENGTH]; //meno hráča, ktoré si zvoli na začiatku hry
    char symbol;    //symbol hráča, ktoré si zvoli na začiatku hry
    SharedNames* names;
    SocketManager* manager;
    sem_t* client_sem;
} Player;


void start_client(SharedNames* names);
Player* player_init(int ID_, char name_[MAX_NAME_LENGTH], char symbol, SharedNames* names, SocketManager* manager);
Player *connect_client(char* serverName, int setup, SharedNames* names);
void* start_server_wrapper(void* arg);
void play_game(char* serverName, int setup, SharedNames* names);
void* make_move(void* arg);
void* update_board(void* arg);

#endif 