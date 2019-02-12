CC = gcc
CFLAGS = -g -Wall -std=c89
CFLAGS2 = -g -Wall -std=c89 -lpthread

bld : bocaletm.buildrooms.c
	$(CC) $(CFLAGS) -o $@ $^

adv : bocaletm.adventure.c
	$(CC) $(CFLAGS2) -o $@ $^

memcheckBld : 
	valgrind --leak-check=yes --show-reachable=yes ./bld

memcheckAdv : 
	valgrind --leak-check=yes --show-reachable=yes ./adv

clean :
	-rm *.o
	-rm bld
	-rm adv
	-find . -type d -name "bocaletm*" -exec rm -r "{}" \;
