CC = gcc
LDFLAGS =
CFLAGS =  -Wall -Werror -Wextra -g

EXEC = main

all : $(EXEC)

%:%.o
	$(CC) -o $@ $^ $(LDFLAGS)

main : projet.o

%.o : %.c
	$(CC) $(CFLAGS) -c  $<

.PHONY : all clean fclean

clean :
	rm -rf *.o *~ $(EXEC) *.a
