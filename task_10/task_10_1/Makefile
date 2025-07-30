BIN := main
OBJ := main.o
SRC := main.c

CC := gcc
.PHONY : all clean

all : $(BIN)

$(BIN): $(OBJ)
		$(CC) $^ -o $@

%.o: %.c
		$(CC) $^ -c

clean:
		rm -rf $(BIN) $(OBJ)