# Definovanie názvov spustiteľných súborov
TARGET_SERVER := server
TARGET_CLIENT := client
TARGET := piskvorky

# Zoznam objektových súborov pre server a klienta
SERVER_OBJS := server.o common.o game_logic.o shared_data.o shared_names.o shm.o socket_manager.o addons.o
CLIENT_OBJS := client.o common.o game_logic.o shared_data.o shared_names.o shm.o socket_manager.o addons.o
TARGET_OBJS := main.o $(SERVER_OBJS) $(CLIENT_OBJS)

# Názov kompilátora a príkazy
CC := gcc
CFLAGS += -std=c99 -O3 -Wall -Wextra -Wpedantic
LDFLAGS :=

# Kompilácia servera
server: $(SERVER_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# Kompilácia klienta
client: $(CLIENT_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# Cieľ pre celý projekt (vytvorenie piskvorky)
all: $(TARGET)

$(TARGET): $(TARGET_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# Čistenie predošlých kompilovaných súborov
clean:
	$(RM) -f *.o $(TARGET) $(TARGET_SERVER) $(TARGET_CLIENT)

# Kompilácia objektových súborov
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

# Označenie pre ne-skutočné ciele
.PHONY: all clean server client
