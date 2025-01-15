#include "shared_data.h"
#include <stdlib.h>
#include <stdio.h>

void shared_data_init(SharedNames* shared_names) {

    if (sem_open(shared_names->mut_pc_, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1) == SEM_FAILED) {
        perror("Failed to create Mutex");
        exit(EXIT_FAILURE);
    }

    if (sem_open(shared_names->sem_consume_, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0) == SEM_FAILED) {
        perror("Failed to create Konzument");
        exit(EXIT_FAILURE);
    }

    if (sem_open(shared_names->sem_produce_, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1) == SEM_FAILED) {
        perror("Failed to create Producent");
        exit(EXIT_FAILURE);
    }


 }

void shared_data_destroy(SharedNames* shared_names) {

    if (sem_unlink(shared_names->mut_pc_) == -1) {
        perror("Failed to unlink Mutex");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink(shared_names->sem_consume_) == -1) {
        perror("Failed to unlink Konzument");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink(shared_names->sem_produce_) == -1) {
        perror("Failed to unlink Producent");
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

    this->sem_produce_ = sem_open(shared_names->sem_produce_, O_RDWR);
    if (this->sem_produce_ == SEM_FAILED) {
        perror("Failed to open producent");
        exit(EXIT_FAILURE);
    }

    this->sem_consume_ = sem_open(shared_names->sem_consume_, O_RDWR);
    if (this->sem_consume_ == SEM_FAILED) {
        perror("Failed to open Producent");
        exit(EXIT_FAILURE);
    }


}


void shared_data_close(SharedData *this) {
    server_shm_close(this->fd_, this->game);

    if (sem_close(this->mut_pc_) == -1) {
        perror("Failed to close Mutex");
        exit(EXIT_FAILURE);
    }

    if (sem_close(this->sem_produce_) == -1) {
        perror("Failed to close Producent");
        exit(EXIT_FAILURE);
    }

    if (sem_close(this->sem_consume_) == -1) {
        perror("Failed to close Konzument");
        exit(EXIT_FAILURE);
    }
}

void sh_add_client(SharedData* sharedData) {
    sem_wait(sharedData->sem_produce_);
    sem_wait(sharedData->mut_pc_);

    sem_post(sharedData->mut_pc_);
    sem_wait(sharedData->sem_consume_);
}
void sh_make_move(SharedData* sharedData) {
    sem_wait(sharedData->sem_produce_);
    sem_wait(sharedData->mut_pc_);

    sem_post(sharedData->mut_pc_);
    sem_wait(sharedData->sem_consume_);
}

void sh_receive_board(SharedData* sharedData) {
    sem_wait(sharedData->sem_consume_);
    sem_post(sharedData->mut_pc_);

    sem_post(sharedData->mut_pc_);
    sem_wait(sharedData->sem_produce_);
}

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


