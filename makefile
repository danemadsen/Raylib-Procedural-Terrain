CC = gcc
CFLAGS = -Wall -std=c99
LDFLAGS = -lraylib -L/usr/local/lib -Wl,-rpath=/usr/local/lib -lm
PROJECTNAME = InfiniteTerrain

main:
	$(CC) $(CFLAGS) main.c -o $(PROJECTNAME)$(LDFLAGS)

testing:
	rm -f testing *.o
	$(CC) $(CFLAGS) testing.c -o testing $(LDFLAGS) -g

.PHONY: clean
clean:
	rm -f $(PROJECTNAME) *.o