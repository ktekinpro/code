CC = gcc
CFLAGS =  -Wall -Wextra -Werror
EXEC = c_and_c
LIBS = -I ../Lib_flux_exec -L ../Lib_flux_exec -lfluxthread -I ../Lib_ipc/ -L ../Lib_ipc -lipc -I ../Lib_reseau/ -L ../Lib_reseau -ltcp -ludp -lpthread

all : c_and_c.c
	$(CC) -o c_and_c c_and_c.c $(CFLAGS) $(LIBS)

debug : c_and_c.c
	$(CC) -o c_and_c c_and_c.c -g $(CFLAGS) $(LIBS)

passupload : c_and_c.c
	$(CC) -o c_and_c c_and_c.c -DPASSUPLOAD $(CFLAGS) $(LIBS)

clean :
	rm -f $(EXEC)
