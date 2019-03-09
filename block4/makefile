CC = gcc
CFLAGS = -g -Wall -std=c99
CFLAGS2 = -g -Wall -std=c99 -lpthread

keygen : keygen.c
	$(CC) $(CFLAGS) -o $@ $^

enc_d : otp_enc_d.c
	$(CC) $(CFLAGS) -o $@ $^

enc : otp_enc.c
	$(CC) $(CFLAGS) -o $@ $^

dec_d : otp_dec_d.c
	$(CC) $(CFLAGS) -o $@ $^

dec : otp_dec.c
	$(CC) $(CFLAGS) -o $@ $^

memcheckKey : 
	valgrind --leak-check=yes --show-reachable=yes ./keygen

memcheckEncd : 
	valgrind --leak-check=yes --show-reachable=yes ./enc_d

memcheckEnc : 
	valgrind --leak-check=yes --show-reachable=yes ./enc

memcheckDecd : 
	valgrind --leak-check=yes --show-reachable=yes ./dec_d

memcheckDec : 
	valgrind --leak-check=yes --show-reachable=yes ./dec

clean :
	-if [ -f *.o ]; then rm *.o 2> /dev/null; fi
	-if [ -f enc_d ]; then rm enc_d 2> /dev/null; fi
	-if [ -f enc ]; then rm enc 2> /dev/null; fi
	-if [ -f dec_d ]; then rm dec_d 2> /dev/null; fi
	-if [ -f dec ]; then rm dec 2> /dev/null; fi
	-if [ -f keygen ]; then rm keygen 2> /dev/null; fi
