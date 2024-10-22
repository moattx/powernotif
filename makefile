CC=clang
CFLAGS=-std=c17 -Wall -Wextra

powernotif:
				$(CC) $(CFLAGS) powernotif.c -o $@

install: powernotif
				install powernotif /usr/local/bin

clean:
				rm powernotif
