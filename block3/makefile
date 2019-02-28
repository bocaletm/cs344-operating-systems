CC = gcc
CFLAGS = -g -Wall

smallsh : smallsh.c
	$(CC) $(CFLAGS) -o $@ $^

memchecksh : 
	valgrind --leak-check=yes --show-reachable=yes ./smallsh

clean :
	-rm smallsh
