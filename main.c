#include "addons.h"
#include "client.h"
#include "shared_data.h"
#include <stdio.h>
#include <stdlib.h>

int main() {

    printStartMenu();
    int choice;
    scanf("%d", &choice);
    SharedNames names;

    if (choice == 1) {
        start_client(&names);
        clearNames(&names);
    } else if (choice == 2) {
        char* serverName = malloc(MAX_NAME_LENGTH * sizeof(char));;
        printf("Zadaj názov pre server: ");
        scanf("%s", serverName);
        createNames(serverName, &names);
        play_game(serverName, 1, &names);

        free(serverName);
        clearNames(&names);
    } else if (choice == 3) {
        return 0;
    }

    return 0;
}

