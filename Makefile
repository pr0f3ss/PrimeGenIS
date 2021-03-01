# usage: make FILENAME=example target

CC = gcc

CFLAGS = -std=gnu99

INCLUDES = -I/usr/compilelibs/openssl/include -L/usr/compilelibs/openssl/lib -Wl,-rpath=/usr/compilelibs/openssl/lib

all: $(FILENAME)

$(FILENAME): $(FILENAME).c
	$(CC) $(CFLAGS) $(INCLUDES) $(FILENAME).c -o $(FILENAME) -lcrypto
