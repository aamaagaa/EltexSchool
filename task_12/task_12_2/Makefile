BIN := client server
CC := gcc
CFLAGS := -Wall -Wextra
.PHONY : all clean

all: $(BIN)

client: client.o
	$(CC) $(CFLAGS) $^ -o $@

server: server.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN) *.o