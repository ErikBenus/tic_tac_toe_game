#ifndef CLIENT_H
#define CLIENT_H

typedef struct Player {
    int ID; //číslo, ktorý je vporadí, každý hráč bude mať unikátne od 0+
    char* name; //meno hráča, ktoré si zvoli na začiatku hry
    char symbol;    //symbol hráča, ktoré si zvoli na začiatku hry
} Player;


void start_client();

#endif 