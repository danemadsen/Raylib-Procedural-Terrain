CC = gcc
CFLAGS = -Wall -std=c99
LDFLAGS = -lraylib -L/usr/local/lib -Wl,-rpath=/usr/local/lib -lm

main: main.c
	$(CC) $(CFLAGS) -o main main.c $(LDFLAGS)

.PHONY: clean
clean:
	rm -f main
