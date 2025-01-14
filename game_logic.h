#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H
 
#define MAX_PLAYER_COUNT 10   //max pocet hracov na hru
#define MAX_NAME_LENGTH 30    
 
typedef struct SharedData {
    int size;                // veľkosť hracej plochy
    int win_condition;       // počet políčok potrebných na výhru
    int num_players;         // počet hráčov
    int current_player;      // index aktuálneho hráča
    int input_index;         // index, kde sa vkladá nový hráč pripojený k hre ??nemozeme pouzit current_player??
    char player_symbols[MAX_PLAYER_COUNT]; // symboly hráčov (maximálne 10 hráčov)
    char player_names[MAX_PLAYER_COUNT][MAX_NAME_LENGTH]; // mená hráčov (maximálne 10 hráčov, max. dĺžka mena 30 znakov)
    char **board;            // hracia plocha
    int winner;              // id víťaza
} SharedData;
 
//štruktúra pre ťah hráča 
typedef struct Move {
    int row; //riadok
    int col; //stĺpec
} Move;

GameLogic* create_game_logic(int size, int win_condition, int num_players, char *symbols, char names[10][50]);
void destroy_game_logic(GameLogic *game);
void print_board(int size_, char **board_);
int make_move(GameLogic *game, int row, int col);
int check_winner(GameLogic *game);
 
#endif