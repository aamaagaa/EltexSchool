BIN := main
CC := gcc
LDFLAGS := -lrt
CFLAGS := -Wall -Wextra
.PHONY : all clean

all: $(BIN)

main: main.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN) *.o