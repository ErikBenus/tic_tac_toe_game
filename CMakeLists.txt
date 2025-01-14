# Minimálna verzia CMake
cmake_minimum_required(VERSION 3.10)

# Nastavenie názvu projektu
project(
    TIC_TAC_TOE
    VERSION 1
    DESCRIPTION "Game Tic-tac-toe"
    LANGUAGES C
)

# Nastavenie jazyka C
set(CMAKE_C_STANDARD 99)

# Zoznam všetkých zdrojových súborov
set(SOURCES
    main.c
    addons.c
)

# Vytvorenie spustiteľného súboru
add_executable(TIC_TAC_TOE ${SOURCES})

