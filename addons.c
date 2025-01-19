#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void printTitle() {
    const char *title = "PIŠKVORKY";
    const int consoleWidth = 70; 
    const int titleLength = strlen(title);
    const int padding = (consoleWidth - titleLength) / 2;

    // Horný rám
    printf("\n");
    for (int i = 0; i < consoleWidth; i++) printf("=");
    printf("\n");

    // Stred s nadpisom
    printf("|");
    for (int i = 1; i < consoleWidth - 1; i++) {
        if (i == padding) {
            printf("%s", title);
            i += titleLength - 1;
        } else {
            printf(" ");
        }
    }
    printf("|\n");

    // Dolný rám
    for (int i = 0; i < consoleWidth; i++) printf("=");
    printf("\n\n");
}

void printStartMenu() {
    printTitle();

    printf("Vitajte v hre Piškvorky pre viacerých hráčov!\n");
    printf("1. Vytvoriť hru\n");
    printf("2. Pripojiť sa k hre\n");
    printf("3. Ukončiť aplikáciu\n");
    printf("-----------------------------------\n");
    printf("Vyberte možnosť: \n");

};

