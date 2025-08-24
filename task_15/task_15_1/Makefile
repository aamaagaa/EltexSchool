BIN := main signal
CC := gcc
CFLAGS := -Wall -Wextra
.PHONY : all clean

all: $(BIN)

main: main.o
	$(CC) $(CFLAGS) $^ -o $@

signal: signal.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN) *.o