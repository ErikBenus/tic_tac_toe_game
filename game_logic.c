#include <stdio.h>
#include <stdlib.h>
#include "game_logic.h"
#include <string.h>


void game_init(GameLogic* game, int size, int win_condition, int num_players) {
    game->size = size;
    game->win_condition = win_condition;
    game->num_players = num_players;
    game->current_player = 0;
    game->game_over = 0;

    // Inicializácia hracej plochy
    for (int i = 0; i <= size; i++) {
        for (int j = 0; j <= size; j++) {
            game->board[i][j] = (i == 0 || j == 0) ? ' ' : '-'; // Prvé riadky/stĺpce sú na číslovanie
        }
    }

    // Číslovanie riadkov a stĺpcov
    for (int i = 1; i <= size; i++) {
        game->board[0][i] = '0' + i; // Hlavička stĺpcov
        game->board[i][0] = '0' + i; // Hlavička riadkov
    }
}



void print_board(int size_, char board_[MAX_BOARD_SIZE][MAX_BOARD_SIZE]) {
    system("clear");
    // Vypíše prvý riadok s číslami
    printf("   ");  // Medzera pre zarovnanie
    for (int j = 1; j <= size_; j++) {
        printf("%2d ", j);  
    }
    printf("\n");

    // Vypíše riadky s číslami a hodnotami
    for (int i = 1; i <= size_; i++) {
        printf("%2d ", i);  
        for (int j = 1; j <= size_; j++) {
            printf("%2c ", board_[i][j]);  
        }
        printf("\n");
    }
}

int make_move_logic(Move move, GameLogic* game) {
    // Posun o 1 kvôli číslovaniu
    if (move.row < 1 || move.row > game->size || move.col < 1 || move.col > game->size || game->board[move.row][move.col] != '-') {
        return 0; // Neplatný ťah
    }
    return 1; // Platný ťah
}

int check_winner(GameLogic *game) {
    int size = game->size;
    int win = game->win_condition;
    char current_symbol = game->player_symbols[game->current_player];

    // Kontrola riadkov, stĺpcov a diagonál
    for (int i = 1; i <= size; i++) {
        for (int j = 1; j <= size; j++) {
            if (game->board[i][j] == current_symbol) {
                // Horizontálne
                if (j + win - 1 <= size) {
                    int count = 0;
                    for (int k = 0; k < win; k++) {
                        if (game->board[i][j + k] == current_symbol) count++;
                    }
                    if (count == win) return 1;
                }

                // Vertikálne
                if (i + win - 1 <= size) {
                    int count = 0;
                    for (int k = 0; k < win; k++) {
                        if (game->board[i + k][j] == current_symbol) count++;
                    }
                    if (count == win) return 1;
                }

                // Diagonálne (doprava dole)
                if (i + win - 1 <= size && j + win - 1 <= size) {
                    int count = 0;
                    for (int k = 0; k < win; k++) {
                        if (game->board[i + k][j + k] == current_symbol) count++;
                    }
                    if (count == win) return 1;
                }

                // Diagonálne (doprava hore)
                if (i - win + 1 >= 1 && j + win - 1 <= size) {
                    int count = 0;
                    for (int k = 0; k < win; k++) {
                        if (game->board[i - k][j + k] == current_symbol) count++;
                    }
                    if (count == win) return 1;
                }
            }
        }
    }
    return 0;
}
