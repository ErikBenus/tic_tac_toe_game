#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H
 
#define MAX_NAME_LENGTH 30    

#include "shared_names.h" 

typedef struct Move {
    int row; 
    int col; 
    char symbol; 
} Move;

typedef struct GameLogic {
    int size;                // veľkosť hracej plochy
    int win_condition;       // počet políčok potrebných na výhru
    int num_players;         // počet hráčov
    int current_player;      // index aktuálneho hráča
    char player_symbols[MAX_PLAYERS]; // symboly hráčov (maximálne 10 hráčov)
    char player_names[MAX_PLAYERS][MAX_NAME_LENGTH]; // mená hráčov (maximálne 10 hráčov, max. dĺžka mena 30 znakov)
    char board[MAX_BOARD_SIZE][MAX_BOARD_SIZE];            // hracia plocha
    int winner;              // id víťaza
    int game_over;           // kontrola viťaza/konca hry
    Move move;               // štruktúra pre ťah hráča
} GameLogic;
 

void print_board(int size_, char board_[MAX_BOARD_SIZE][MAX_BOARD_SIZE]);
int make_move_logic(Move move, GameLogic* game);
int check_winner(GameLogic *game);
void game_init(GameLogic* game, int size, int win_condition, int num_players);
 
#endif