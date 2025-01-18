#ifndef CLIENT_H
#define CLIENT_H

#include "shm.h"
#include "game_logic.h"
#include "server.h"

typedef struct Player {
    int ID; //číslo, ktorý je vporadí, každý hráč bude mať unikátne od 0+
    char name[30]; //meno hráča, ktoré si zvoli na začiatku hry
    char symbol;    //symbol hráča, ktoré si zvoli na začiatku hry
    SharedNames* names;
    SocketManager* manager;
} Player;


void start_client(SharedNames* names);
Player* player_init(int ID_, char name_[30], char symbol, SharedNames* names, SocketManager* manager);
void player_destroy(Player* player);
Player *connect_client(char* serverName, int setup, SharedNames* names);
void* start_server_wrapper(void* arg);
void play_game(char* serverName, int setup, SharedNames* names);
void* make_move(void* arg);
void* update_board(void* arg);

#endif 