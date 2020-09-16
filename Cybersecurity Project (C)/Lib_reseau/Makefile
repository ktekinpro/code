CC = gcc
CFLAGS =  -Wall -Wextra -Werror
LIBS = -I ../Lib_flux_exec -L ../Lib_flux_exec -lfluxthread -I ../Lib_ipc/ -L ../Lib_ipc -lipc -I . -L . -ltcp -ludp -lpthread -ldl

all : libudp.c libtcp.c
	$(CC) -c libudp.c $(CFLAGS)
	ar r libudp.a libudp.o
	$(CC) -c libtcp.c $(CFLAGS)
	ar r libtcp.a libtcp.o

clean :
	rm -f *.o *.a

gitclean :
	git rm -f *.o *.a
