BIN := client server
CC := gcc
LDFLAGS := -lrt
CFLAGS := -Wall -Wextra
.PHONY : all clean

all: $(BIN)

client: client.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDLAGS)

server: server.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN) *.o