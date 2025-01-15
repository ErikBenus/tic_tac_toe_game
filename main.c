#include "addons.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>

int main() {

    printStartMenu();
    int choice;
    scanf("%d", &choice);

    if(choice == 1) {
        start_client();
    }

    if(choice == 2) {
        connect_client();
    }

    return 0;
}