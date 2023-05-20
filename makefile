CC = gcc
CFLAGS = -Wall -std=c99
LDFLAGS = -lraylib -L/usr/local/lib -Wl,-rpath=/usr/local/lib -lm
PROJECTNAME = InfiniteTerrain

main:
	$(CC) $(CFLAGS) -c terrain.c -o terrain.o $(LDFLAGS)
	$(CC) $(CFLAGS) -c main.c -o main.o $(LDFLAGS)
	$(CC) $(CFLAGS) main.o terrain.o -o $(PROJECTNAME) $(LDFLAGS)
	rm -f *.o

testing:
	$(CC) $(CFLAGS) testing.c -o testing $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(PROJECTNAME) *.o