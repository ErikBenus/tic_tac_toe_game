#ifndef SHARED_NAMES
#define SHARED_NAMES

typedef struct SharedNames
{
    char *shm_;
    char *mut_pc_;
    char serverName[30];
    char *startGame;
    char *update_board;
    char *move_sem;
} SharedNames;

void clearNames(SharedNames* names);
char * add_suffix(const char * name, const char * suffix);
void createNames(const char* suffix, SharedNames* names);
#endif 