CC = gcc
CFLAGS = -Wall

object = *.c
target = dksh

.PHONY: all debug clean
all: $(target)

$(target): $(object)

debug:
	$(CC) $(CFLAGS) -g -o $(target) $(object)

clean:
	rm -f $(target)
