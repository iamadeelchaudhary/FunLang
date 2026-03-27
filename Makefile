CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2 -g
SRC = src/main.c src/lexer.c src/parser.c src/ast.c src/env.c src/eval.c src/value.c src/builtins.c src/error.c
OBJ = $(SRC:.c=.o)
TARGET = funlang

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) data.txt

run: $(TARGET)
	@if [ -z "$(FILE)" ]; then \
		echo "Usage: make run FILE=examples/hello.fun"; \
	else \
		./$(TARGET) $(FILE); \
	fi