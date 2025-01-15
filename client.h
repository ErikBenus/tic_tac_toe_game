#ifndef CLIENT_H
#define CLIENT_H

#include "shm.h"
#include "game_logic.h"
#include "server.h"

typedef struct Player {
    int ID; //číslo, ktorý je vporadí, každý hráč bude mať unikátne od 0+
    char name[30]; //meno hráča, ktoré si zvoli na začiatku hry
    char symbol;    //symbol hráča, ktoré si zvoli na začiatku hry
} Player;


void start_client();
Player* player_init(int ID_, char name_[30], char symbol);
void player_destroy(Player* player);
void connect_client();

#endif 