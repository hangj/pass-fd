CC = clang
CFLAGS = -g

all: a.out

a.out: main.c
	$(CC) $(CFLAGS) -o $@ $^
