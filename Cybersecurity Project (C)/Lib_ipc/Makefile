CC = gcc
CFLAGS =  -Wall -Wextra -Werror

all : libipc.c
	$(CC) -c libipc.c $(CFLAGS)
	ar r libipc.a libipc.o

clean :
	rm -f *.o *.a
	ipcrm -a

gitclean :
	git rm -f *.o *.a
