#ifndef SHARED_DATA_H
#define SHARED_DATA_H
 
#define MAX_PLAYER_COUNT 10   //max pocet hracov na hru
#define MAX_NAME_LENGTH 30    

#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include "shm.h"
#include "game_logic.h"
 
 
typedef struct SharedData {
    GameLogic* game;
    int fd_;
    sem_t *mut_pc_;
    sem_t *sem_produce_;
    sem_t *sem_consume_;
} SharedData;


void shared_data_init(SharedNames* shared_names);
void shared_data_destroy(SharedNames* shared_names);
void shared_data_open(SharedData *this, SharedNames* shared_names);
void shared_data_close(SharedData *this);

void sh_add_client(SharedData* sharedData);
void sh_make_move(SharedData* sharedData);
void sh_receive_board(SharedData* sharedData);
int symbol_exists(char symbol);
int player_exists(char* name);



#endif