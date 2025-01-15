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
 
/*void process_special_command(const char* input, Server* server) {
    if (strncmp(input, "-h", 2) == 0) {
        // Príkaz pre help
        printf("HELP: Zadajte riadok a stĺpec pre ťah, alebo použite príkazy:\n");
        printf("-h: zobraziť tento pomocník\n");
        printf("-e: odísť zo hry\n");
        printf("-s: uložiť hru (implementácia neskôr)\n");
    } else if (strncmp(input, "-e", 2) == 0) {
        // Príkaz pre leave
        printf("Ukončenie hry\n");
        exit(0); 
    } else if (strncmp(input, "-s", 2) == 0) {
        // Príkaz pre save
        printf("Zadajte názov súboru pre uloženie hry: ");
        char filename[100];
        scanf("%s", filename);
        save_game_to_file(server->game, filename);
    } else {
        // Ak sa nezadaný príkaz nenachádza, ignorujeme
        printf("Neznámy príkaz: %s\n", input);
    }
}*/
