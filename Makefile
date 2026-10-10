CC     = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Werror -g
OBJ    = main.o runner.o
TARGET = pipeline

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c runner.h
	$(CC) $(CFLAGS) -c $< -o $@

asan: CFLAGS += -fsanitize=address,undefined -fno-omit-frame-pointer
asan: clean $(TARGET)

test: all
	sh test.sh

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all asan test clean
