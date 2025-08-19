BIN := client server
CC := gcc
LDFLAGS := -lrt -lncurses -lpthread
CFLAGS = -pthread
.PHONY : all clean

all: $(BIN)

client: client.o handler.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

server: server.o handler.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

handler: handler.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -rf $(BIN) *.o