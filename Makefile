CC = gcc
CFLAGS = -Wall -std=c99
ANSICFLAGS = -Wall -ansi
DEBUGFLAGS = -g

object = *.c
target = dksh

.PHONY: all debug ansi ansi-debug clean
all: $(target)

$(target): $(object)

debug:
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -o $(target) $(object)

ansi:
	$(CC) $(ANSICFLAGS) -o $(target) $(object)

ansi-debug:
	$(CC) $(ANSICFLAGS) $(DEBUGFLAGS) -o $(target) $(object)

clean:
	rm -f $(target)
	rm -rf dksh.dSYM/
