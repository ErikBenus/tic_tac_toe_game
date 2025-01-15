#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "shared_data.h"
#include "socket_manager.h"
#include "client.h"

// void* make_move(void* arg) {
//     Player player;
//     Move move;

//     printf("%s, zadaj riadok a stĺpec svojho ťahu: ", player.name);
//     while(scanf("%d %d", &move.row, &move.col) != 2) {
//         printf("Nesprávny zápis. Skús to znova: \n");
//     }

//     while(!make_move) {
//         printf("Vybral si nevhodné poličko. Skús to znova: \n");
//             while(scanf("%d %d", &move.row, &move.col) != 2) {
//             printf("Nesprávny zápis. Skús to znova: \n");
//     }
//         //close(client_sock);
//     }

// }

void start_client() {
 
    char serverName[30];
    int size_;
    int num_players_;
    int win_condition_;

    printf("Zadaj názov pre server %d: ");
    scanf("%s", &serverName);
    start_server(serverName);

    printf("Zadajte počet hráčov (2-10): ");
    scanf("%d", &num_players_);
    while (num_players_ < 2 || num_players_ > 10) {
        printf("Neplatný počet hráčov! Zadajte hodnotu medzi 2 a 10: ");
        scanf("%d", &num_players_);
    }

    //zadanie velkosti hracieho pola
    printf("Zadajte veľkosť hracieho poľa (napr. 3 pre 3x3): ");
    scanf("%d", &size_);
    while (size_ < 3 || size_ > 100) {
        printf("Neplatná veľkosť! Zadajte hodnotu medzi 3 a 100: ");
        scanf("%d", &size_);
    }

    //pocet policok ptorenych na vitazstvo
    printf("Zadajte počet políčok potrebných na víťazstvo (min.3 a max.%d): ", size_);
    scanf("%d", &win_condition_);
    while (win_condition_ < 3 || win_condition_ > size_) {
        printf("Neplatná veľkosť! Zadajte hodnotu medzi 3 a %d)", size_);
        scanf("%d", &win_condition_);
    }

    connect_client();
}

void connect_client() {
    Player* player;
    SocketManager* client = create_socket(8080, 0);
    if (!client) {
        fprintf(stderr, "Failed to create client socket\n");
        return EXIT_FAILURE;
    }

    if (connect(client->socket_fd, (struct sockaddr*)&client->address, sizeof(client->address)) < 0) {
        perror("Connection failed");
        close_socket(client);
        return EXIT_FAILURE;
    }

    char player_name[30];
    char player_symbol;
    int player_ID;

    printf("Zadaj svoje meno %d: ");
    scanf("%s", player_name);
    
    if (send_data(client->socket_fd, player_name, strlen(player_name) + 1) < 0) {
        perror("Failed to send player name");
        close_socket(client);
        return EXIT_FAILURE;
    }

    printf("Zadaj svoj symbol(jeden znak): ");
    scanf(" %c", &player_symbol);

    if (send_data(client->socket_fd, &player_symbol, sizeof(player_symbol)) < 0) {
        perror("Failed to send player symbol");
        close_socket(client);
        return EXIT_FAILURE;
    }

    if (receive_data(client->socket_fd, &player_ID, sizeof(player_ID)) > 0) {
        player = player_init(player_ID, player_name, player_symbol);       
    } else {
        perror("Failed to receive number from server");
    }

}

Player* player_init(int ID_, char name_[30], char symbol) {
    Player* new_player = malloc(sizeof(Player));
    if (new_player == NULL) {
        perror("Failed to allocate memory for Player");
        return NULL;  // Ak sa nepodarí alokovať pamäť, vráti sa NULL.
    }

    new_player->ID = ID_;
    strcpy(new_player->name, name_); 
    new_player->symbol = symbol;

    return new_player;
}

void player_destroy(Player* player) { 
    free(player);
}