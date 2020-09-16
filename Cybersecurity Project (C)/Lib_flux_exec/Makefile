CC = gcc
CFLAGS =  -Wall -Wextra -Werror

all : libfluxthread.c
	$(CC) -c libfluxthread.c $(CFLAGS)
	ar r libfluxthread.a libfluxthread.o

clean :
	rm -f *.o *.a

gitclean :
	git rm -f *.o *.a
