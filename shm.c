#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "shm.h"


void shm_init(SharedNames names){
    shm_unlink(name);           
    int shm_fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR); // Vytvoriť zdieľanú pamäť
    if (shm_fd == -1) {
        perror("Nemôžem vytvoriť zdieľanú pamäť");
        exit(EXIT_FAILURE);
    }

    // Nastavenie veľkosti zdieľanej pamäte
    if (ftruncate(shm_fd, sizeof(GameLogic)) == -1) {
        perror("Nemôžem nastaviť veľkosť zdieľanej pamäti");
        close(shm_fd);
        exit(EXIT_FAILURE);
    }

    // Mapovanie zdieľanej pamäti do adresného priestoru
    server->shm_ptr = mmap(NULL, sizeof(GameLogic), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
}

void shm_destroy(SharedNames names) {
    // Odstrániť zdieľanú pamäť
    if (shm_unlink(name) == -1) {
        perror("Nemôžem odstrániť zdieľanú pamäť");
    }
}


void server_shm_open(SharedNames names, int *out_fd_shm) {
    const int shm_fd = shm_open(name, O_RDWR, 0);
    if (shm_fd == -1) {
        perror("Failed to open shared memory");
        exit(EXIT_FAILURE);
    }
    server->shm_ptr = mmap(NULL, sizeof(GameLogic), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (server->shm_ptr == MAP_FAILED) {
        perror("Failed to map shared memory");
        exit(EXIT_FAILURE);
    }
    *out_fd_shm = shm_fd;
}


void server_shm_close(SharedNames names) {
    if (munmap(server->shm_ptr, sizeof(GameLogic)) == -1) {
        perror("Failed to unmap shared memory");
        exit(EXIT_FAILURE);
    }
    if (close(shm_fd) == -1) {
        perror("Failed to close shared memory");
        exit(EXIT_FAILURE);
    }
}