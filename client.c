#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "shared_data.h"
#include "socket_manager.h"
#include "client.h"


void start_client(SharedNames* names) {
    char* serverName = malloc(MAX_NAME_LENGTH * sizeof(char));;
    char player_name[MAX_NAME_LENGTH];
    printf("Zadaj názov pre server: ");
    scanf("%s", serverName);
    int socket_name = hashString(serverName);
    strcpy(names->serverName, serverName);  
    createNames(serverName, names);
    
    SyncData sync_data;
    pthread_mutex_init(&sync_data.mutex, NULL);  
    pthread_cond_init(&sync_data.cond, NULL);    
    sync_data.server_ready = 0;
 

    ThreadArgs args;
    args.names = names;
    args.sync_data = &sync_data;
 
    pthread_t server_thread;
    if (pthread_create(&server_thread, NULL, start_server_wrapper, (void*)&args) != 0) {
        perror("Failed to create server thread");
        return;
    }
   
    pthread_mutex_lock(&sync_data.mutex);
    while (sync_data.server_ready == 0) {
        pthread_cond_wait(&sync_data.cond, &sync_data.mutex);  
    }
    pthread_mutex_unlock(&sync_data.mutex);
 
    pthread_mutex_destroy(&sync_data.mutex);  
    pthread_cond_destroy(&sync_data.cond);
    play_game(serverName, 0, names);
    pthread_join(server_thread, NULL);  
    free(serverName);
}


Player* connect_client(char* serverName, int setup, SharedNames* names) {
    int socket_name = hashString(serverName);
    int current_player = setup;
    GameLogic game_logic;
    Player* player;
    SocketManager* client = create_socket(socket_name, 0);
    if (!client) {
        fprintf(stderr, "Failed to create client socket\n");
        exit(EXIT_FAILURE);  
    }
    
    // Pripojenie k serveru
    if (connect(client->socket_fd, (struct sockaddr*)&client->address, sizeof(client->address)) < 0) {
        perror("Connection failed");
        close_socket(client);
        exit(EXIT_FAILURE);  
    }

    char player_name[MAX_NAME_LENGTH];
    char player_symbol;
    int player_ID;

    // Získanie mena hráča
    printf("Zadaj svoje meno: ");
    scanf("%s", player_name);
    
    // Odoslanie mena na server
    if (send_data(client->socket_fd, player_name, strlen(player_name) + 1) < 0) {
        perror("Failed to send player name");
        close_socket(client);
        exit(EXIT_FAILURE);  
    }

    // Získanie symbolu hráča
    printf("Zadaj svoj symbol (jeden znak): ");
    scanf(" %c", &player_symbol); 

    SharedData sh;
    shared_data_open(&sh, names);
    int is_unique = 0; 

    while (!is_unique) {
        is_unique = 1;  
        for (int i = 0; i < MAX_PLAYERS; i++) {  
            if (sh.game->player_symbols[i] == player_symbol) {
                is_unique = 0;  
                break;
            }
        }

        if (!is_unique) {
            printf("Zadaný symbol už existuje! Zadaj iný symbol: ");
            scanf(" %c", &player_symbol);
            }
    }
    int num_players = sh.game->num_players;
    shared_data_close(&sh, num_players);

    // Odoslanie symbolu na server
    if (send_data(client->socket_fd, &player_symbol, sizeof(player_symbol)) < 0) {
        perror("Failed to send player symbol");
        close_socket(client);
        exit(EXIT_FAILURE);
    }

    // Prijatie hráčovho ID od servera
    if (receive_data(client->socket_fd, &player_ID, sizeof(player_ID)) > 0) {
        player = player_init(player_ID, player_name, player_symbol, names, client);
    } else {
        perror("Failed to receive player ID from server");
        close_socket(client);
        exit(EXIT_FAILURE); 
    }

    close_socket(client); 
    return player;
}

void* start_server_wrapper(void* arg) {
    ThreadArgs* args = arg;  
    start_server(args->names, args->sync_data);  
    return NULL;
}

Player* player_init(int ID_, char name_[MAX_NAME_LENGTH], char symbol, SharedNames* names, SocketManager* manager) {
    Player* new_player = malloc(sizeof(Player));
    if (new_player == NULL) {
        perror("Failed to allocate memory for Player");
        return NULL;  
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
    int num_players = shared_data.game->num_players;
    Player* player = connect_client(serverName, setup, names);

    printf("\n -- Čaká sa na pripojenie všetkých hráčov... -- \n");

    sem_wait(shared_data.startGame);

    player->client_sem = malloc(sizeof(sem_t));
    if (player->client_sem == NULL) {
        perror("Failed to allocate memory for client_sem");
        exit(EXIT_FAILURE);
    }
 
    if (sem_init(player->client_sem, 0, 1) == -1) { 
        perror("Failed to initialize client_sem semaphore");
        exit(EXIT_FAILURE);
    }
   
    int si = shared_data.game->size; 
    print_board(si, shared_data.game->board); 

    pthread_t moveThread, drawThread;
    pthread_create(&moveThread, NULL, &make_move, player);
    pthread_create(&drawThread, NULL, &update_board, player);

    // Čakanie na ukončenie hry
    pthread_join(moveThread, NULL);
    pthread_join(drawThread, NULL);
    shared_data_close(&shared_data, num_players);

    if (sem_destroy(player->client_sem) == -1) {
        perror("Failed to destroy client_sem semaphore");
        exit(EXIT_FAILURE);
    }

    free(player->client_sem);
    free(player);
}


void* make_move(void* arg) {
    Player* player = (Player*)arg;
    Move move_ = {0}; 
    SharedData shared;
    int game_over = 0;
    int num_players;

    while (game_over == 0) {  
        shared_data_open(&shared, player->names);
        num_players = shared.game->num_players;
        sem_wait(player->client_sem);

        game_over = shared.game->game_over;
        if (game_over == 1) {
            printf("\n*********************************************************************\n");
            printf("Koniec hry!\n");
            int winner = shared.game->winner;
            printf("Hru vyhral hráč %s so znakom %c\n", shared.game->player_names[winner], shared.game->player_symbols[winner]);
            printf("\n*********************************************************************\n");
            shared_data_close(&shared, num_players);
            break; 
        }

       if (shared.game->current_player == player->ID) {
            printf("%s, je tvoj ťah! \nZadaj riadok a stĺpec (0 0 - pre ukončenie): ", player->name);
            scanf("%d %d", &move_.row, &move_.col);
            int move_status = make_move_logic(move_, shared.game);

            if(move_.row == 0 && move_.col == 0) {
                shared.game->player_symbols[player->ID] = '-'; 
                shared.game->move.symbol = '-'; 
                shared_data_close(&shared, num_players);
                sem_post(shared.move_sem);
                return NULL;
            } else {
           
            while(!move_status) {
                printf("%s, zadal si neplatný ťah! \nZadaj validný riadok a stĺpec (0 0 - pre ukončenie): ", player->name);
                scanf("%d %d", &move_.row, &move_.col);
                move_status = make_move_logic(move_, shared.game);
            }
 
            move_.symbol = player->symbol;
            shared.game->move = move_;
            shared_data_close(&shared, num_players);
            sem_post(shared.move_sem);
            }
        } else {
            printf("Nie si na rade! Čakaj...\n");
            shared_data_close(&shared, num_players);
        }

        usleep(500000);
    }

    return NULL;
}



void* update_board(void* arg) {
    Player* player = (Player*)arg;
    SharedData s;
    int game_over = 0;

    while (game_over == 0) {
        shared_data_open(&s, player->names);

        sem_wait(s.player_sems[player->ID]);
        game_over = s.game->game_over;
        int si = s.game->size;
        int num_players = s.game->num_players;
        print_board(si, s.game->board);
        sem_post(player->client_sem);
        shared_data_close(&s, num_players);    
    }
    return NULL;
}









