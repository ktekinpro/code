CC = gcc
CFLAGS =  -Wall -Wextra -Werror
EXEC = admin
LIBS = -I ../Lib_flux_exec -L ../Lib_flux_exec -lfluxthread -I ../Lib_ipc/ -L ../Lib_ipc -lipc -I ../Lib_reseau/ -L ../Lib_reseau -ltcp -ludp -lpthread


all : admin.c
	$(CC) -o admin admin.c $(CFLAGS) $(LIBS)

debug : admin.c
	$(CC) -o admin admin.c -g $(CFLAGS) $(LIBS)

clean :
	rm -f $(EXEC)
