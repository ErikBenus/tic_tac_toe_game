#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "server.h"
#include "shared_data.h"

void start_server(SharedNames* names) {
    GameLogic* game_setup = malloc(sizeof(GameLogic));
    char serverName[30];
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
    game_init(game_setup, size_, win_condition_, num_players_);
    shm_init(names, game_setup);
    printf("plocha: %d počet hračov: %d\n", game_setup->size, game_setup->num_players);

    SharedData shared_data;
    //shared_data_open(&shared_data, names);

    add_client(server, num_players_, names);
    shared_data_open(&shared_data, names);
    shared_data_close(&shared_data);
    // Čakanie na ukončenie vlákna spracúvajúceho klientov

    
    ThreadArgs* thread_args = (ThreadArgs*)malloc(sizeof(ThreadArgs));
        if (!thread_args) {
            perror("Failed to allocate memory for thread arguments");
        }
        thread_args->names = names;

    pthread_t playing_clients[num_players_];
    if (pthread_create(&playing_clients[num_players_], NULL, client_thread_function, (void*)thread_args) != 0) {
            perror("Failed to create thread for handling client");
            free(thread_args);
        }
    
    close_socket(server);  // Uzavretie serverového socketu po skončení slučky
}

// Funkcia na spracovanie pripojení klientov (prijímanie pripojení)
void add_client(SocketManager* server, int num_players, SharedNames* names) {
    //SocketManager* server = (SocketManager*)arg;  // Server dostane socket z hlavnej funkcie
    int client_fd;
    if (!server) {
        fprintf(stderr, "Failed to create server socket\n");
        exit(EXIT_FAILURE);
    }

    int actualNumber = 0;
    pthread_t client_threads[num_players];
    while (actualNumber < num_players) {  // Server neustále čaká na pripojenie nového klienta
        socklen_t addr_len = sizeof(server->address);
        client_fd = accept(server->socket_fd, (struct sockaddr*)&server->address, &addr_len);
        if (client_fd < 0) {
            perror("Accept failed");
            continue; // Pokračujeme v čakaní na ďalšie pripojenie
        }

        // Vytvorenie nového vlákna pre každého klienta
       if (pthread_create(&client_threads[actualNumber], NULL, handle_client, (void*)&client_fd) != 0) {
            perror("Failed to create thread for handling client");
            close(client_fd);
            continue; // Pokračujeme v čakaní na ďalšie pripojenie
        }

        actualNumber++;
    }
 
    for (int i = 0; i < num_players; i++) {
        pthread_join(client_threads[i], NULL); // Čaká na ukončenie každého klientského vlákna
    }
    SharedData shared_data;
    shared_data_open(&shared_data, names);
    for (int i = 0; i < shared_data.game->num_players; ++i) {
        sem_post(shared_data.startGame);  // Uvoľnenie semaforu pre každého hráča
    }
 // Uvoľní semafor startGame, ktorý signalizuje, že všetci klienti sú pripravení na hru
    shared_data_close(&shared_data);
}


void* handle_client(void* arg) {
    int client_fd = *((int*)arg);
    char player_name[30];
    char player_symbol;
    int player_ID;
    static int current_player = 0; // Static premenná pre inkrmentáciu ID
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
    player_ID = current_player++;  // Tento riadok je zodpovedný za inkrmentáciu ID
    pthread_mutex_unlock(&player_mutex);
    printf("Player %s with symbol %c has been assigned ID: %d\n", player_name, player_symbol, player_ID);

    // Posielanie ID klientovi
    if (send_data(client_fd, &player_ID, sizeof(player_ID)) < 0) {
        perror("Failed to send player ID");
    }

    // Uzavretie spojenia
    close(client_fd);
    return NULL;
}

// Funkcia, ktorá sa spustí vo vlákne pre každého klienta
void* client_thread_function(void* args) {
    ThreadArgs* thread_args = (ThreadArgs*)args;
    SharedNames* names = thread_args->names;

    // Spracovanie ťahov klienta
    process_client_moves(names);

    // Uvoľnenie zdrojov
    free(thread_args);

    return NULL;
}

void process_client_moves(SharedNames* names) {
    Move move;
    SharedData shared_data;
    

    // Otvorenie zdieľanej pamäte
    while (1) {
    shared_data_open(&shared_data, names);
    int current_player = shared_data.game->current_player; 

        // Prijímanie ťahu od hráča
        sem_wait(shared_data.move_sem);

                // Zamkni prístup k zdieľanej pamäti
                sem_wait(shared_data.mut_pc_);
                move = shared_data.game->move;
                // Zapíš ťah do zdieľanej pamäti
                shared_data.game->board[move.row][move.col] = move.symbol;

                // Skontroluj, či nie je víťaz
                if (check_winner(shared_data.game)) {
                    shared_data.game->game_over = 1;
                    shared_data.game->winner = shared_data.game->current_player;
                    shared_data_close(&shared_data);
                    // Odošli signál všetkým klientom, že došlo k zmene stavu
                    for (int i = 0; i < shared_data.game->num_players; ++i) {
                        sem_post(shared_data.update_board);  // Uvoľnenie semaforu pre každého hráča
                    }

                    break;
                }

                // Zmeň aktuálneho hráča
                shared_data.game->current_player = (shared_data.game->current_player + 1) % shared_data.game->num_players;
                // Odošli signál všetkým klientom o zmene stavu hry (napr. výmena hráča)
                // Odomkni prístup k zdieľanej pamä
                // Ak je hra ukončená, ukonči vlákno
                if (shared_data.game->game_over) {
                    printf("Game over. Closing connection for client.\n");
                    break;
                }

                for (int i = 0; i < shared_data.game->num_players; ++i) {
                    sem_post(shared_data.update_board);  // Uvoľnenie semaforu pre každého hráča
                }

                shared_data_close(&shared_data);
            }
        
    

    // Uzavretie spojenia a vyčistenie zdrojov
}


// void process_client_moves(SharedNames* names, ClientManager* cm) {
//     Move move;
//     SharedData shared_data;

//     // Otvorenie zdieľanej pamäte
//     shared_data_open(&shared_data, names);

//     while (1) {
//         // Prijímanie ťahu od klienta
//         if (receive_data(client_fd, &move, sizeof(Move)) <= 0) {
//             perror("Failed to receive move");
//             break;  // Ukonči vlákno pri chybe alebo odpojení klienta
//         }

//         // Zamkni prístup k zdieľanej pamäti
//         sem_wait(shared_data.mut_pc_);

//         // Zapíš ťah do zdieľanej pamäti
//         shared_data.game->board[move.row][move.col] = move.symbol;

//         // Skontroluj, či nie je víťaz
//         if (check_winner(shared_data.game)) {
//             shared_data.game->game_over = 1;
//             shared_data.game->winner = shared_data.game->current_player;

//             // Odošli signál všetkým klientom, že došlo k zmene stavu
//             broadcast_change_signal(cm);

//             // Ukonči cyklus, pretože hra je ukončená
//             sem_post(shared_data.mut_pc_);
//             break;
//         }

//         // Zmeň aktuálneho hráča
//         shared_data.game->current_player = (shared_data.game->current_player + 1) % shared_data.game->num_players;

//         // Odošli signál všetkým klientom o zmene stavu hry (napr. výmena hráča)
//         broadcast_change_signal(cm);

//         // Odomkni prístup k zdieľanej pamäti
//         sem_post(shared_data.mut_pc_);

//         // Ak je hra ukončená (napr. víťazstvo/remíza), ukonči vlákno
//         if (shared_data.game->game_over) {
//             printf("Game over. Closing connection for client.\n");
//             break;
//         }
//     }

//     // Uzavretie spojenia a vyčistenie zdrojov
//     close(client_fd);
//     shared_data_close(&shared_data);
// }


// Bude potrebné potom vypnúť server a teda uzavrieť jeho sokcet, napríkald keď sa koční hra (bude víťaž alebo proste všetci odídu)

// Akov yreišiť kapacitu je vo wode na ploche POS_add clients kapacita


void handle_player_move(int client_fd, SharedNames* shared) {
   /* Move move;
    int move_status = 0;

    // Prijímanie ťahu od klienta (hráča)
    if (receive_data(client_fd, &move, sizeof(move)) < 0) {
        perror("Failed to receive move");
        return;
    }

    // Semafor: Zámok na zdieľanú pamäť, aby sa zabránilo konfliktom
    sem_wait(shared->mut_pc_);

    // Získaj aktuálneho hráča
    int current_player = shared->game->current_player;

    // Skontroluj, či je tento hráč ten, ktorý má právo hrať
    if (move.row < 0 || move.row >= shared->game->size || move.col < 0 || move.col >= shared->game->size) {
        // Neplatný ťah (mimo hranice)
        move_status = 0;
    } else if (shared->game->board[move.row][move.col] != ' ') {
        // Poličko už je obsadené
        move_status = 0;
    } else if (current_player == shared->game->current_player) {
        // Platný ťah, zapíš na hernú plochu
        shared->game->board[move.row][move.col] = shared->game->player_symbols[current_player];
        
        // Zmena na ďalšieho hráča
        shared->game->current_player = (current_player + 1) % shared->game->num_players;

        // Nastav stav ťahu ako úspešný
        move_status = 1;
    } else {
        // Tento hráč nie je na rade
        move_status = 0;
    }

    // Semafor: Uvoľni prístup k zdieľanej pamäti
    sem_post(shared->mut_pc_);

    // Pošli späť status ťahu (úspech alebo neúspech)
    if (send_data(client_fd, &move_status, sizeof(move_status)) < 0) {
        perror("Failed to send move status");
    }*/
}
