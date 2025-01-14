# Názov kompilátora
CC = gcc

# Príznaky pre kompilátor
CFLAGS = -Wall -Wextra -std=c99

# Zoznam zdrojových súborov
SOURCES = main.c addons.c

# Výsledný spustiteľný súbor
EXEC = my_program

# Kompilácia hlavného programu
$(EXEC): $(SOURCES)
	$(CC) $(CFLAGS) -o $(EXEC) $(SOURCES)

# Čistenie predošlých kompilovaných súborov
clean:
	rm -f $(EXEC)