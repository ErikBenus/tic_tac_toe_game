#ifndef SHM_H
#define SHM_H

#include "game_logic.h"
#include "shared_names.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


void shm_init(SharedNames* names);
void shm_destroy(SharedNames* names);
void server_shm_open(SharedNames* names, GameLogic **outSharedData, int *out_fd_shm);
void server_shm_close(int fd, GameLogic* sharedData);

#endif