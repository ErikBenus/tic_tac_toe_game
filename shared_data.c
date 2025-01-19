#include "shared_data.h"
#include <stdlib.h>
#include <stdio.h>

void shared_data_init(SharedNames* shared_names, int num_players) {
    
     if (sem_open(shared_names->mut_pc_, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, 1) == SEM_FAILED) {
        perror("Failed to initialize mut_pc_ semaphore");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_players; i++) {
        if (sem_open(shared_names->player_sems[i], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, 0)== SEM_FAILED) {
            perror("Failed to initialize semaphore");
            exit(EXIT_FAILURE);
        }
    }

    if (sem_open(shared_names->startGame, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, 0) == SEM_FAILED) {
        perror("Failed to initialize startGame semaphore");
        exit(EXIT_FAILURE);
    }

    if (sem_open(shared_names->move_sem, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, 0) == SEM_FAILED) {
        perror("Failed to initialize move semaphore");
        exit(EXIT_FAILURE);
    }

 }

void shared_data_destroy(SharedNames* shared_names, int num_players) {

    if (sem_unlink(shared_names->mut_pc_) == -1) {
        perror("Failed to unlink Mutex");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink(shared_names->startGame) == -1) {
            perror("Failed to unlink Mutex");
            exit(EXIT_FAILURE);
        }

    if (sem_unlink(shared_names->move_sem) == -1) {
            perror("Failed to unlink Mutex");
            exit(EXIT_FAILURE);
        }    

    for (int i = 0; i < num_players; i++) {
        if(sem_unlink(shared_names->player_sems[i]) == -1) {
            perror("Failed to unlink player semaphore");
            exit(EXIT_FAILURE);
        }
    }
 }


void shared_data_open(SharedData *this, SharedNames* shared_names) {
    server_shm_open(shared_names, &this->game, &this->fd_); 
    this->mut_pc_ = sem_open(shared_names->mut_pc_, O_RDWR);
    if (this->mut_pc_ == SEM_FAILED) {
        perror("Failed to open Mutex");
        exit(EXIT_FAILURE);
    }

    this->startGame = sem_open(shared_names->startGame, O_RDWR);
    if (this->startGame == SEM_FAILED) {
        perror("Failed to open Start Game");
        exit(EXIT_FAILURE);
    }

    this->move_sem = sem_open(shared_names->move_sem, O_RDWR);
    if (this->move_sem == SEM_FAILED) {
        perror("Failed to open Update Board");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < this->game->num_players; i++) {
        char sem_name[MAX_SEM_LENGTH];
        sprintf(sem_name, "player_sem_%d", i);

        this->player_sems[i] = sem_open(shared_names->player_sems[i], O_RDWR);
        if (this->player_sems[i] == SEM_FAILED) {
            perror("Failed to open player semaphore");
            exit(EXIT_FAILURE);
        }
    }

}


void shared_data_close(SharedData *this, int num_players) {
    server_shm_close(this->fd_, this->game);

    if (sem_close(this->mut_pc_) == -1) {
        perror("Failed to close Mutex");
        exit(EXIT_FAILURE);
    }

    if (sem_close(this->startGame) == -1) {
        perror("Failed to close Start Game");
        exit(EXIT_FAILURE);
    }

    if (sem_close(this->move_sem) == -1) {
        perror("Failed to close Start Game");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_players; i++) {
        if(sem_close(this->player_sems[i]) == -1) {
            perror("Failed to close player semaphore");
            exit(EXIT_FAILURE);
        }
    }

}


