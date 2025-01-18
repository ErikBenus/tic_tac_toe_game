#include "addons.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>

int main() {

    printStartMenu();
    int choice;
    scanf("%d", &choice);
    SharedNames names;


    if(choice == 1) {
        start_client(&names);
    }

    if(choice == 2) {
        //play_game();
        char* serverName = malloc(30 * sizeof(char));;
        printf("Zadaj názov pre server: ");
        scanf("%s", serverName);
        createNames(serverName, &names);
        play_game(serverName, 1, &names);
    }

    return 0;
}

