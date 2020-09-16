CC = gcc
CFLAGS =  -Wall -Wextra -Werror
EXEC = serveur
LIBS = -I ../Lib_flux_exec -L ../Lib_flux_exec -lfluxthread -I ../Lib_ipc/ -L ../Lib_ipc -lipc -I ../Lib_reseau/ -L ../Lib_reseau -ltcp -ludp -lpthread


all : serveur.c
	$(CC) -o serveur serveur.c $(CFLAGS) $(LIBS)

debug : serveur.c
	$(CC) -o serveur serveur.c -g $(CFLAGS) $(LIBS)

clean :
	rm -f *.o *~ *.a $(EXEC) *.hex *.elf
