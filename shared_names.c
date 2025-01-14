
#include "shared_names.c"
#include <string.h>
#include <stdlib.h>


void clearNames(SharedNames* names){
    free(names->shm_);
    free(names->semMutex_);
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
  // možno ptorbený FREE k result
}

void createNames(const char* suffix, SharedNames names){
    names->shm_ = add_suffix("SHM", suffix);
    names->semMutex_ = add_suffix("SEM-MUTEX", suffix);
}
