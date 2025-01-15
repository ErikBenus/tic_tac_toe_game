#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "shm.h"
#include "game_logic.h"


void shm_destroy(SharedNames* names) {
    // Odstrániť zdieľanú pamäť
    if (shm_unlink(names->shm_) == -1) {
        perror("Nemôžem odstrániť zdieľanú pamäť");
        exit(EXIT_FAILURE);
    }
}

void shm_init(SharedNames* names) {
    const int shm_fd = shm_open(names->shm_, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        perror("Nemôžem vytvoriť zdieľanú pamäť");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, sizeof(GameLogic)) == -1) {
        perror("Nemôžem nastaviť veľkosť zdieľanej pamäti");
        exit(EXIT_FAILURE);
    }

    GameLogic* shm_ptr = mmap(NULL, sizeof(GameLogic), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("Nemôžem mapovať zdieľanú pamäť");
        exit(EXIT_FAILURE);
    }

    // Inicializácia údajov
    // shm_ptr->size = shared_data->size;
    // shm_ptr->win_condition = shared_data->win_condition;
    // shm_ptr->num_players = shared_data->num_players;
    // shm_ptr->current_player = 0;
    // shm_ptr->winner = -1;

    // shm_ptr->board = malloc(shm_ptr->size * sizeof(char*));
    // for (int i = 0; i < shm_ptr->size; i++) {
    //     shm_ptr->board[i] = calloc(shm_ptr->size, sizeof(char));
    // }

    // // Vytvorenie semaforov
    // sem_t* sem_write = sem_open(names->semMutex_, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
    // if (sem_write == SEM_FAILED) {
    //     perror("Nemôžem vytvoriť semafor");
    //     shm_unlink(names->shm_);
    //     exit(EXIT_FAILURE);
    // }

    munmap(shm_ptr, sizeof(GameLogic));
    server_shm_close(shm_fd, shm_ptr);
}



void server_shm_open(SharedNames* names, GameLogic **outSharedData, int *out_fd_shm) {
    const int shm_fd = shm_open(names->shm_, O_RDWR, 0);
    if (shm_fd == -1) {
        perror("Nemôžem otvoriť zdieľanú pamäť");
        exit(EXIT_FAILURE);
    }
    GameLogic* shdata = mmap(NULL, sizeof(GameLogic), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shdata == MAP_FAILED) {
        perror("Failed to map shared memory");
        exit(EXIT_FAILURE);
    }
    *out_fd_shm = shm_fd;
    *outSharedData = shdata;

}


void server_shm_close(int fd, GameLogic* sharedData) {
    if (munmap(sharedData, sizeof(GameLogic)) == -1) {
        perror("Failed to unmap shared memory");
        exit(EXIT_FAILURE);
    }
    if (close(fd) == -1) {
        perror("Failed to close shared memory");
        exit(EXIT_FAILURE);
    }
}