CC=clang
CFLAGS=-std=gnu17 -Wall -Wextra

powernotif:
	$(CC) $(CFLAGS) powernotif.c -o $@

clean:
	rm powernotif
