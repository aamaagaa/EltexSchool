BIN := echo-server echo-client
CC := gcc
LDFLAGS := -lrt
CFLAGS := -Wall -Wextra
.PHONY : all clean

all: $(BIN)

echo-client: echo-client.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDLAGS)

echo-server: echo-server.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN) *.o