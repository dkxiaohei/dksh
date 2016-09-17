CC = gcc
CFLAGS = -Wall -Wextra

object = pwd.o cd.o echo.o cat.o ls.o mkdir.o rmdir.o rm.o date.o chmod.o wc.o kill.o history.o who.o help.o exit.o grep.o mv.o tee.o time.o more.o dksh.o
target = dksh

.PHONY: all
all: $(target)

$(target): $(object)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean cleanall
clean:
	rm -f *.o

cleanall: clean
	rm -f $(target)
