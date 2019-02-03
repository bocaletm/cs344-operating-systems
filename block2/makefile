CC = gcc
CFLAGS = -g -Wall -std=c89

test : bocaletm.buildrooms.c
	$(CC) $(CFLAGS) -o $@ $^

memcheckTest : 
	valgrind --leak-check=yes --show-reachable=yes ./test

clean :
	-rm *.o
	-rm test
	-find . -type d -name "bocaletm*" -exec rm -r "{}" \;
