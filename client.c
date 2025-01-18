#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "shared_data.h"
#include "socket_manager.h"
#include "client.h"


void start_client(SharedNames* names) {
    char* serverName = malloc(30 * sizeof(char));;
    char player_name[30];
    printf("Zadaj názov pre server: ");
    scanf("%s", serverName);
    int socket_name = hashString(serverName);
    strcpy(names->serverName, serverName);  
    createNames(serverName, names);
    pthread_t server_thread;
    if (pthread_create(&server_thread, NULL, start_server_wrapper, (void*)names) != 0) {
        perror("Failed to create server thread");
        return;
    }
    sleep(10);
    play_game(serverName, 0, names);
    //connect_client(serverName, 0);
}


Player* connect_client(char* serverName, int setup, SharedNames* names) {
    int socket_name = hashString(serverName);
    int current_player = setup;
    GameLogic game_logic;
    Player* player;
    SocketManager* client = create_socket(socket_name, 0);
    if (!client) {
        fprintf(stderr, "Failed to create client socket\n");
        exit(EXIT_FAILURE);  // Ukončí program pri chybe
    }
    
    // Pokus o pripojenie k serveru
    if (connect(client->socket_fd, (struct sockaddr*)&client->address, sizeof(client->address)) < 0) {
        perror("Connection failed");
        close_socket(client);
        exit(EXIT_FAILURE);  // Ukončí program pri chybe
    }

    char player_name[30];
    char player_symbol;
    int player_ID;

    // Získanie mena hráča
    printf("Zadaj svoje meno: ");
    scanf("%s", player_name);
    
    // Odoslanie mena na server
    if (send_data(client->socket_fd, player_name, strlen(player_name) + 1) < 0) {
        perror("Failed to send player name");
        close_socket(client);
        exit(EXIT_FAILURE);  // Ukončí program pri chybe
    }

    // Získanie symbolu hráča
    printf("Zadaj svoj symbol (jeden znak): ");
    scanf(" %c", &player_symbol);  // " " pred %c zabezpečí, že sa skonzumuje predchádzajúci znak nového riadku

    // Odoslanie symbolu na server
    if (send_data(client->socket_fd, &player_symbol, sizeof(player_symbol)) < 0) {
        perror("Failed to send player symbol");
        close_socket(client);
        exit(EXIT_FAILURE);  // Ukončí program pri chybe
    }

    // Prijatie hráčovho ID od servera
    if (receive_data(client->socket_fd, &player_ID, sizeof(player_ID)) > 0) {
        player = player_init(player_ID, player_name, player_symbol, names, client);
    } else {
        perror("Failed to receive player ID from server");
        close_socket(client);
        exit(EXIT_FAILURE);  // Ukončí program pri chybe
    }
    printf("Volam sa %s so znakom %c a ID %d a som pripravený hrať \n", player->name, player->symbol, player->ID);
    return player;
}

//obaľovacia kvôli warningu
void* start_server_wrapper(void* arg) {
    SharedNames* server = arg;
    start_server(server);
    return NULL;
}

Player* player_init(int ID_, char name_[30], char symbol, SharedNames* names, SocketManager* manager) {
    Player* new_player = malloc(sizeof(Player));
    if (new_player == NULL) {
        perror("Failed to allocate memory for Player");
        return NULL;  // Ak sa nepodarí alokovať pamäť, vráti sa NULL.
    }

    new_player->ID = ID_;
    strcpy(new_player->name, name_); 
    new_player->symbol = symbol;
    new_player->names = names;
    new_player->manager = manager;

    return new_player;
}



void play_game(char* serverName, int setup, SharedNames* names) {
    SharedData shared_data;
    shared_data_open(&shared_data, names);
    Player* player = connect_client(serverName, setup, names);
    sem_wait(shared_data.startGame);
    printf("hra sa moze zacat!\n");

    pthread_t moveThread, drawThread;
    pthread_create(&moveThread, NULL, &make_move, player);
    pthread_create(&drawThread, NULL, &update_board, player);

    // Čakanie na ukončenie hry
    pthread_join(moveThread, NULL);
    pthread_join(drawThread, NULL);

}


void* make_move(void* arg) {
    Player* player = (Player*)arg;
    Move move_;
    int move_status = 0;
    SharedData shared;
    int already_notified = 0;  // Flag na kontrolu, či bola správa už vypísaná

    while (1) {  // Zamkni prístup k zdieľanej pamäti
        shared_data_open(&shared, player->names);
        sem_wait(shared.mut_pc_);

        // Skontroluj, či je hra ukončená
        if (shared.game->game_over) {
            shared_data_close(&shared);
            sem_post(shared.mut_pc_);  // Odomkni semafor
            break;  // Ukonči vlákno
        }

        // Skontroluj, či je hráč na rade
        if (shared.game->current_player == player->ID) {
            // Ak je hráč na rade, resetuj flag a dovoľ vykonať ťah
            already_notified = 0;
            printf("%s, je tvoj ťah! Zadaj riadok a stĺpec (napr. 1 2): ", player->name);
            scanf("%d %d", &move_.row, &move_.col);
            move_.symbol = player->symbol;

            shared.game->move = move_;
            // Odoslanie ťahu na server
            shared_data_close(&shared);

            sem_post(shared.move_sem);
            // Ak bol ťah spracovaný, ukonči cyklus pre daný ťah
            move_status = 1;
        } else {
            // Ak hráč nie je na rade a správa ešte nebola vypísaná
            if (!already_notified) {
                printf("Nie si na rade! Čakám...\n");
                already_notified = 1;  // Nastav flag, že sme už informovali
            }
        }

        sem_post(shared.mut_pc_);  // Odomkni semafor

        // Krátka pauza pred opakovaním
        usleep(500000);
    }

    return NULL;
}





//Vlákno na zobrazovane aktuálneho boardu
// void* update_board(void* arg) {
//     Player* player = (Player*)arg;

//     SharedData s;
//     sem_wait(shared_data.mut_pc_);
//     shared_data_open(&s, player->names); 
//     int si = s.game->size; 
//     print_board(si, s.game->board);
//     shared_data_close(&s);
//     sem_post(shared_data.mut_pc_);
//     return NULL;
// }

void* update_board(void* arg) {
    Player* player = (Player*)arg;
    int change_signal;  // Zmena na int
    SharedData s;
    shared_data_open(&s, player->names);
    while (1) {
        // Čakaj na signál od servera
        sem_wait(s.update_board);
        sem_wait(s.mut_pc_);  // Zamkni prístup k zdieľanej pamäti

        int si = s.game->size;
        print_board(si, s.game->board);

        shared_data_close(&s);
        sem_post(s.mut_pc_);  // Odomkni prístup k zdieľanej pamäti
        
    }

    return NULL;
}




void player_destroy(Player* player) { 
    free(player);
}






