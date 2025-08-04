BIN := main
OBJ := main.o
SRC := main.c

CC := gcc
LDFLAGS := -pthread
.PHONY : all clean

all : $(BIN)

$(BIN): $(OBJ)
		$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c
		$(CC) $^ -c

clean:
		rm -rf $(BIN) $(OBJ)