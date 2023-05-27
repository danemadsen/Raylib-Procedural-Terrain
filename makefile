CC = gcc
CFLAGS = -Wall -std=c99
LDFLAGS = -lraylib -L/usr/local/lib -Wl,-rpath=/usr/local/lib -lm -g
PROJECTNAME = InfiniteTerrain

main:
	$(CC) $(CFLAGS) src/main.c -o $(PROJECTNAME) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(PROJECTNAME) *.o