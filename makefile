CC=clang
CFLAGS=-std=c17 -Wall -Wextra

powernotif:
	$(CC) $(CFLAGS) powernotif.c -o $@

clean:
	rm powernotif
