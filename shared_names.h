#ifndef SHARED_NAMES
#define SHARED_NAMES

#define MAX_PLAYERS 10
#define MAX_BOARD_SIZE 100
#define MAX_NAME_LENGTH 30    

typedef struct SharedNames
{
    char *shm_;
    char *mut_pc_;
    char serverName[MAX_NAME_LENGTH];
    char *startGame;
    char *update_board;
    char *move_sem;
    char* player_sems[MAX_PLAYERS];
} SharedNames;

void clearNames(SharedNames* names);
char * add_suffix(const char * name, const char * suffix);
void createNames(const char* suffix, SharedNames* names);
#endif 