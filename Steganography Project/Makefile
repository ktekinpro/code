CC = gcc
CFLAGS =  -Wall -Wextra -Werror -Os
EXEC = main encrypte decrypte
LIBS = -I. -L. -lstegano

all : encrypte.c decrypte.c
	$(CC) -c libstegano.c $(CFLAGS)
	ar r libstegano.a libstegano.o
	$(CC) -o encrypte encrypte.c $(CFLAGS) $(LIBS)
	$(CC) -o decrypte decrypte.c $(CFLAGS) $(LIBS)

clean :
	rm -f *.o *~ *.a *.hex *.elf $(EXEC)
