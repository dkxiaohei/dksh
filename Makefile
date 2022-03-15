CC = gcc
CFLAGS = -ansi -Wall

object = *.c
target = dksh

.PHONY: all clean
all: $(target)

$(target): $(object)

clean:
	rm -f $(target)
