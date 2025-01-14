#include <stdio.h>
#include <stdlib.h>
#include <pthread_t>
#include <string.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <unistd.h>

void* make_move(void* arg) {
    Player player;
    Move move;

    printf("%s, zadaj riadok a stĺpec svojho ťahu: ", player.name);
    while(scanf("%d %d", &move.row, &move.col) != 2) {
        printf("Nesprávny zápis. Skús to znova: \n");
    }

    while(!make_move) {
        printf("Vybral si nevhodné poličko. Skús to znova: \n");
            while(scanf("%d %d", &move.row, &move.col) != 2) {
            printf("Nesprávny zápis. Skús to znova: \n");
    }
        //close(client_sock);
    }

}

void start_client() {
    printf("Zadaj svoje meno %d: ");
        scanf("%s", player.name);

        printf("Zadaj svoj symbol(jeden znak): ");
        scanf(" %c", player.symbol);

// potrebujem prístup do pamäte:
        // for (int j = 0; j < i; j++) {
        //     while (config.symbols[i] == config.symbols[j]) {
        //         printf("Symbol '%c' je už obsadený! Zadajte iný symbol: ", config.symbols[i]);
        //         scanf(" %c", &config.symbols[i]);
        //     }
        // }
}