#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H
 
#define MAX_PLAYER_COUNT 10   //max pocet hracov na hru
#define MAX_NAME_LENGTH 30    
 

//štruktúra pre ťah hráča 
typedef struct Move {
    int row; //riadok
    int col; //stĺpec
    char symbol; 
} Move;

typedef struct GameLogic {
    int size;                // veľkosť hracej plochy
    int win_condition;       // počet políčok potrebných na výhru
    int num_players;         // počet hráčov
    int current_player;      // index aktuálneho hráča
    int input_index;         // index, kde sa vkladá nový hráč pripojený k hre ??nemozeme pouzit current_player??
    char player_symbols[MAX_PLAYER_COUNT]; // symboly hráčov (maximálne 10 hráčov)
    char player_names[MAX_PLAYER_COUNT][MAX_NAME_LENGTH]; // mená hráčov (maximálne 10 hráčov, max. dĺžka mena 30 znakov)
    char board[100][100];            // hracia plocha
    int winner;              // id víťaza
    int game_over;
    Move move;
} GameLogic;
 

GameLogic* create_game_logic(int size, int win_condition, int num_players);
void print_board(int size_, char board_[100][100]);
int make_move_logic(Move move, GameLogic* game);
int check_winner(GameLogic *game);
void game_init(GameLogic* game, int size, int win_condition, int num_players);
 
#endif