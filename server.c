#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "server.h"
#include "shared_data.h"


void start_server(SharedNames* names, SyncData* sync_data) {
    GameLogic* game_setup = malloc(sizeof(GameLogic));
    char serverName[MAX_NAME_LENGTH];
    strcpy(serverName, names->serverName);
    int socket_name = hashString(serverName);

    SocketManager* server = create_socket(socket_name, 1);  
    if (!server) {
        fprintf(stderr, "Failed to create server socket\n");
        return;
    }
    
    int size_;
    int num_players_;        
    int win_condition_; 
    printf("Zadajte počet hráčov (2-%d): ", MAX_PLAYERS);
    scanf("%d", &num_players_);
    while (num_players_ < 2 || num_players_ > MAX_PLAYERS) {
        printf("Neplatný počet hráčov! Zadajte hodnotu medzi 2 a %d: ", MAX_PLAYERS);
        scanf("%d", &num_players_);
    }
  
    printf("Zadajte veľkosť hracieho poľa (napr. 3 pre 3x3): ");        
    scanf("%d", &size_);
    while (size_ < 3 || size_ > MAX_BOARD_SIZE) {
        printf("Neplatná veľkosť! Zadajte hodnotu medzi 3 a %d: ", MAX_BOARD_SIZE);
        scanf("%d", &size_);
    }
    
    printf("Zadajte počet políčok potrebných na víťazstvo (min.3 a max.%d): ", size_);
    scanf("%d", &win_condition_);
    while (win_condition_ < 3 || win_condition_ > size_) {
        printf("Neplatná veľkosť! Zadajte hodnotu medzi 3 a %d)", size_);
        scanf("%d", &win_condition_);
    }
    game_init(game_setup, size_, win_condition_, num_players_);
    shm_init(names, game_setup);

    pthread_mutex_lock(&sync_data->mutex);
    sync_data->server_ready = 1;  
    pthread_cond_signal(&sync_data->cond);  
    pthread_mutex_unlock(&sync_data->mutex);

    add_client(server, num_players_, names);

    process_client_moves(names);

    close_socket(server);  
    free(game_setup);
}

void add_client(SocketManager* server, int num_players, SharedNames* names) {
    int client_fd;
    if (!server) {
        fprintf(stderr, "Failed to create server socket\n");
        exit(EXIT_FAILURE);
    }
    ClientArgs* client_args;
    int actualNumber = 0;
    pthread_t client_threads[num_players];

    while (actualNumber < num_players) {  
        socklen_t addr_len = sizeof(server->address);
        client_fd = accept(server->socket_fd, (struct sockaddr*)&server->address, &addr_len);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        client_args = (ClientArgs*)malloc(sizeof(ClientArgs));
        if (!client_args) {
            perror("Failed to allocate memory for client arguments");
            close(client_fd);
            continue;
        }
        client_args->client_fd = client_fd;
        client_args->names = names;

        // Vytvorenie nového vlákna pre každého klienta
       if (pthread_create(&client_threads[actualNumber], NULL, handle_client, (void*)client_args) != 0) {
            perror("Failed to create thread for handling client");
            close(client_fd);
            continue; 
        }

        actualNumber++;
    }
 
    for (int i = 0; i < num_players; i++) {
        pthread_join(client_threads[i], NULL); 
    }
    SharedData shared_data;
    shared_data_open(&shared_data, names);
    for (int i = 0; i < shared_data.game->num_players; ++i) {
        sem_post(shared_data.startGame);  
    }

    shared_data_close(&shared_data, num_players);
}


void* handle_client(void* arg) {
    ClientArgs* client_args = (ClientArgs*)arg;
    int client_fd = client_args->client_fd;
    SharedNames* names = client_args->names;
    char player_name[MAX_NAME_LENGTH];
    char player_symbol;
    int player_ID;
    static int current_player = 0; // Static premenná pre inkrementáciu ID
    pthread_mutex_t player_mutex = PTHREAD_MUTEX_INITIALIZER;


    // Prijímanie mena klienta
    memset(player_name, 0, sizeof(player_name));
    if (receive_data(client_fd, player_name, sizeof(player_name)) <= 0) {
        perror("Failed to receive player name");
        close(client_fd);
        return NULL;
    }

    // Prijímanie symbolu klienta
    if (receive_data(client_fd, &player_symbol, sizeof(player_symbol)) <= 0) {
        perror("Failed to receive player symbol");
        close(client_fd);
        return NULL;
    }
    
    // Vytvorenie unikátneho ID pre klienta
    pthread_mutex_lock(&player_mutex);
    player_ID = current_player++;  // Tento riadok je zodpovedný za inkrementáciu ID
    pthread_mutex_unlock(&player_mutex);
   
    SharedData shared_data;
    shared_data_open(&shared_data, names);
    int num_players = shared_data.game->num_players;
    shared_data.game->player_symbols[player_ID] = player_symbol;

    strncpy(shared_data.game->player_names[player_ID], player_name, MAX_NAME_LENGTH - 1);
    shared_data.game->player_names[player_ID][MAX_NAME_LENGTH - 1] = '\0';

    shared_data_close(&shared_data, num_players);

    // Posielanie ID klientovi
    if (send_data(client_fd, &player_ID, sizeof(player_ID)) < 0) {
        perror("Failed to send player ID");
    }

    close(client_fd);
    free(client_args);
    return NULL;
}



void process_client_moves(SharedNames* names) {
    Move move = {0}; 
    SharedData shared_data;
    int game_over = 0;
    int num_players;
    
    while (game_over == 0) {
        shared_data_open(&shared_data, names);
        num_players = shared_data.game->num_players;

        sem_wait(shared_data.move_sem);
        
    if (shared_data.game->player_symbols[shared_data.game->current_player] != '-') {
        move = shared_data.game->move;
        // Zapíš ťah do zdieľanej pamäti
        shared_data.game->board[move.row][move.col] = move.symbol;

        game_over = check_winner(shared_data.game);
             if (game_over == 1) {
                shared_data.game->game_over = 1;
                shared_data.game->winner = shared_data.game->current_player;

                for (int i = 0; i < shared_data.game->num_players; i++) {
                    sem_post(shared_data.player_sems[i]);
                }
                shared_data_close(&shared_data, num_players);
                break;
            }
        shared_data.game->current_player = (shared_data.game->current_player + 1) % shared_data.game->num_players;
    }

    while (shared_data.game->player_symbols[shared_data.game->current_player] == '-') {
            // Preskoči aktuálneho hráča
            shared_data.game->current_player = (shared_data.game->current_player + 1) % shared_data.game->num_players;
        }
  
        for (int i = 0; i < shared_data.game->num_players; ++i) {
                sem_post(shared_data.player_sems[i]);
        }
            shared_data_close(&shared_data, num_players);
        }
}

