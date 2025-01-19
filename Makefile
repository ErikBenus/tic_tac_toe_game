# Definovanie názvov spustiteľných súborov
TARGET_SERVER := server
TARGET_CLIENT := client
TARGET := piskvorky

SERVER_OBJS := server.o common.o game_logic.o shared_data.o shared_names.o shm.o socket_manager.o addons.o
CLIENT_OBJS := client.o common.o game_logic.o shared_data.o shared_names.o shm.o socket_manager.o addons.o
TARGET_OBJS := main.o $(SERVER_OBJS) $(CLIENT_OBJS)

CC := gcc
CFLAGS += -std=c99 -O3 -Wall -Wextra -Wpedantic
LDFLAGS :=

server: $(SERVER_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

client: $(CLIENT_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

all: $(TARGET)

$(TARGET): $(TARGET_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	$(RM) -f *.o $(TARGET) $(TARGET_SERVER) $(TARGET_CLIENT)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

.PHONY: all clean server client
