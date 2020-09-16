CC = gcc
CFLAGS =  -Wall -Wextra -Werror
EXEC = bot
LIBS = -I ../Lib_flux_exec -L ../Lib_flux_exec -lfluxthread -I ../Lib_ipc/ -L ../Lib_ipc -lipc -I ../Lib_reseau/ -L ../Lib_reseau -ltcp -ludp -lpthread -ldl


all : bot.c
	$(CC) -o bot bot.c $(CFLAGS) $(LIBS)

debug : bot.c
	$(CC) -o bot bot.c -g $(CFLAGS) $(LIBS)

clean :
	rm -f $(EXEC)
