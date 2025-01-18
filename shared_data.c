#include "shared_data.h"
#include <stdlib.h>
#include <stdio.h>

void shared_data_init(SharedNames* shared_names) {
    
     if (sem_open(shared_names->mut_pc_, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, 1) == SEM_FAILED) {
        perror("Failed to initialize mut_pc_ semaphore");
        exit(EXIT_FAILURE);
    }

    // Initialize update_board semaphore
    if (sem_open(shared_names->update_board, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, 0) == SEM_FAILED) {
        perror("Failed to initialize update_board semaphore");
        exit(EXIT_FAILURE);
    }

    // Initialize startGame semaphore
    if (sem_open(shared_names->startGame, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, 0) == SEM_FAILED) {
        perror("Failed to initialize startGame semaphore");
        exit(EXIT_FAILURE);
    }

    if (sem_open(shared_names->move_sem, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, 0) == SEM_FAILED) {
        perror("Failed to initialize startGame semaphore");
        exit(EXIT_FAILURE);
    }

    // if (sem_open(shared_names->mut_pc_, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1) == SEM_FAILED) {
    //     perror("Failed to create Mutex");
    //     exit(EXIT_FAILURE);
    // } 

    // if (sem_open(shared_names->update_board, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0) == SEM_FAILED) {
    //     perror("Failed to create Mutex");
    //     exit(EXIT_FAILURE);
    // } 
    
    // if (sem_open(shared_names->startGame, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0) == SEM_FAILED) {
    //     perror("Failed to create startGame semaphore");
    //     exit(EXIT_FAILURE);
    // }


 }

void shared_data_destroy(SharedNames* shared_names) {

    if (sem_unlink(shared_names->mut_pc_) == -1) {
        perror("Failed to unlink Mutex");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink(shared_names->update_board) == -1) {
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

 }


void shared_data_open(SharedData *this, SharedNames* shared_names) {
    server_shm_open(shared_names, &this->game, &this->fd_); //treba tu tie && ???
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

    this->update_board = sem_open(shared_names->update_board, O_RDWR);
    if (this->update_board == SEM_FAILED) {
        perror("Failed to open Update Board");
        exit(EXIT_FAILURE);
    }

    this->move_sem = sem_open(shared_names->move_sem, O_RDWR);
    if (this->move_sem == SEM_FAILED) {
        perror("Failed to open Update Board");
        exit(EXIT_FAILURE);
    }
}


void shared_data_close(SharedData *this) {
    server_shm_close(this->fd_, this->game);

    if (sem_close(this->mut_pc_) == -1) {
        perror("Failed to close Mutex");
        exit(EXIT_FAILURE);
    }

    if (sem_close(this->update_board ) == -1) {
        perror("Failed to Update Board");
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

}

// void sh_add_client(SharedData* sharedData) {
//     sem_wait(sharedData->sem_produce_);
//     sem_wait(sharedData->mut_pc_);

//     sem_post(sharedData->mut_pc_);
//     sem_wait(sharedData->sem_consume_);
// }
// void sh_make_move(SharedData* sharedData) {
//     sem_wait(sharedData->sem_produce_);
//     sem_wait(sharedData->mut_pc_);

//     sem_post(sharedData->mut_pc_);
//     sem_wait(sharedData->sem_consume_);
// }

// void sh_receive_board(SharedData* sharedData) {
//     sem_wait(sharedData->sem_consume_);
//     sem_post(sharedData->mut_pc_);

//     sem_post(sharedData->mut_pc_);
//     sem_wait(sharedData->sem_produce_);
//}

// int player_exists(SharedData *this, char* name) {
//     for (int i = 0; i < this->game->num_players; i++) {
//         if (strcmp(this->game->player_names[i], name) == 0) {
//             return 1; // Meno už existuje
//         }
//     }
//     return 0; // Meno neexistuje
// }

// int symbol_exists(SharedData *this, char symbol) {
//     for (int i = 0; i < this->game->num_players; i++) {
//         if (this->game->player_symbols[i] == symbol) {
//             return 1; // Symbol už existuje
//         }
//     }
//     return 0; // Symbol neexistuje
// }


