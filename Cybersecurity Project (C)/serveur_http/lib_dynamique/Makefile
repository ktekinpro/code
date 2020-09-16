CC = gcc
CFLAGS =  -Wall -Wextra -Werror

all : charge_test.c
	$(CC) -c charge_test.c $(CFLAGS)
	$(CC) -fPIC -shared -o charge_test.so charge_test.o

clean :
	rm -f *.o *.so

gitclean :
	git rm -f *.o *.so
