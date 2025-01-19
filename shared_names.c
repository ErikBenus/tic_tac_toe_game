
#include "shared_names.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void clearNames(SharedNames* names){
    free(names->shm_);
    free(names->mut_pc_);
    free(names->startGame);
    free(names->move_sem);

    for(int i = 0; i < MAX_PLAYERS; i++) {        
      free(names->player_sems[i]);
    }
}

//Funkcia prebratá z cvičení
char * add_suffix(const char* name, const char* suffix){
  size_t name_len = strlen(name);
  size_t new_len = strlen(suffix);
  char * result = calloc(name_len + new_len + 2, sizeof(char));
  strcpy(result, name);
  result[name_len] = '-';
  strcpy(result + name_len + 1, suffix);
  return result;
}

void createNames(const char* suffix, SharedNames* names){
    names->shm_ = add_suffix("SHM", suffix);
    names->mut_pc_= add_suffix("MUT_PC", suffix);
    names->startGame = add_suffix("START_GAME", suffix);
    names->move_sem = add_suffix("MOVE_SEM", suffix);

    for(int i = 0; i < MAX_PLAYERS; i++) {
      char player_name[MAX_NAME_LENGTH]; 
      snprintf(player_name, sizeof(player_name), "PLAYER%d", i + 1);
      names->player_sems[i] = add_suffix(player_name, suffix);
    }
}
