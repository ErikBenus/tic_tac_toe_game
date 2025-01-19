#ifndef SHARED_DATA_H
#define SHARED_DATA_H
 
#define MAX_SEM_LENGTH 20

#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include "shm.h"
#include "game_logic.h" 
#include "shared_names.h"

typedef struct SharedData {
    GameLogic* game;
    int fd_;
    sem_t* player_sems[MAX_PLAYERS];
    sem_t *mut_pc_;
    sem_t *startGame;
    sem_t *update_board;
    sem_t *move_sem;
} SharedData;

 
typedef struct SyncData{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int server_ready;
} SyncData;


void shared_data_init(SharedNames* shared_names, int num_players);
void shared_data_destroy(SharedNames* shared_names, int num_players);
void shared_data_open(SharedData *this, SharedNames* shared_names);
void shared_data_close(SharedData *this, int num_players);



#endif