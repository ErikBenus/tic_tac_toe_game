#include <stdio.h>
#include <stdlib.h>
#include "game_logic.h"
#include <string.h>

// Vytvorenie hernej logiky
GameLogic* create_game_logic(int size, int win_condition, int num_players) {
    GameLogic *game = malloc(sizeof(GameLogic));
    game->size = size;
    game->win_condition = win_condition;
    game->num_players = num_players;
    game->current_player = 0;

    // // Kopírovanie symbolov a mien hráčov
    // for (int i = 0; i < num_players; i++) {
    //     game->player_symbols[i] = symbols[i];
    //     strcpy(game->player_names[i], names[i]); // Skopírovanie mena
    // }

    // Inicializácia hracej plochy
    game->board = malloc((size + 1) * sizeof(char *)); // +1 kvôli číslovaniu
    for (int i = 0; i <= size; i++) {
        game->board[i] = malloc((size + 1) * sizeof(char)); // +1 kvôli číslovaniu
        for (int j = 0; j <= size; j++) {
            game->board[i][j] = (i == 0 || j == 0) ? ' ' : '-'; // Prvé riadky/stĺpce sú na číslovanie
        }
    }

    // Číslovanie riadkov a stĺpcov
    for (int i = 1; i <= size; i++) {
        game->board[0][i] = '0' + i; // Hlavička stĺpcov
        game->board[i][0] = '0' + i; // Hlavička riadkov
    }

    return game;
}


// Uvoľnenie pamäte
void destroy_game_logic(GameLogic *game) {
    for (int i = 0; i <= game->size; i++) {
        free(game->board[i]);
    }
    free(game->board);
    free(game);
}

// Vykreslenie hracej plochy
void print_board(int size_, char **board_) {
    // Vymaže konzolu
    system("clear");
    // Vypíše prvý riadok s číslami
    printf("   ");  // Medzera pre zarovnanie
    for (int j = 1; j <= size_; j++) {
        printf("%2d ", j);  // Vypíše číslo stĺpca s pevným formátom na 2 miesta
    }
    printf("\n");

    // Vypíše riadky s číslami a hodnotami
    for (int i = 1; i <= size_; i++) {
        printf("%2d ", i);  // Vypíše číslo riadku s pevným formátom na 2 miesta
        for (int j = 1; j <= size_; j++) {
            printf("%2c ", board_[i][j]);  // Vypíše znak, zarovnaný na 2 miesta
        }
        printf("\n");
    }
}



// Vykonanie ťahu
// int make_move(Move move) {
//     // Posun o 1 kvôli číslovaniu
//     GameLogic* game;
//     if (move.row < 1 || move.row > game->size || move.col < 1 || move.col > game->size || game->board[move.row][move.col] != '-') {
//         return 0; // Neplatný ťah
//     }
//     //game->board[row][col] = game->player_symbols[game->current_player];
//     return 1; // Platný ťah
// }

// Kontrola víťaza
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
