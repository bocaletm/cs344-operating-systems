CC = gcc
CFLAGS = -g -Wall -Wextra 
CFLAGS2 = -g -Wall

keygen : keygen.c
	$(CC) $(CFLAGS) -o $@ $^

otp_enc_d : otp_enc_d.c
	$(CC) $(CFLAGS) -o $@ $^

otp_enc : otp_enc.c
	$(CC) $(CFLAGS) -o $@ $^

otp_dec_d : otp_dec_d.c
	$(CC) $(CFLAGS) -o $@ $^

otp_dec : otp_dec.c
	$(CC) $(CFLAGS) -o $@ $^

memcheckKey : 
	valgrind --leak-check=yes --show-reachable=yes ./keygen

memcheckEncd : 
	valgrind --leak-check=yes --show-reachable=yes ./otp_enc_d

memcheckEnc : 
	valgrind --leak-check=yes --show-reachable=yes ./otp_enc

memcheckDecd : 
	valgrind --leak-check=yes --show-reachable=yes ./otp_dec_d

memcheckDec : 
	valgrind --leak-check=yes --show-reachable=yes ./otp_dec

clean :
	-if [ -f *.o ]; then rm *.o 2> /dev/null; fi
	-if [ -f otp_enc_d ]; then rm otp_enc_d 2> /dev/null; fi
	-if [ -f otp_enc ]; then rm otp_enc 2> /dev/null; fi
	-if [ -f otp_dec_d ]; then rm otp_dec_d 2> /dev/null; fi
	-if [ -f otp_dec ]; then rm otp_dec 2> /dev/null; fi
	-if [ -f keygen ]; then rm keygen 2> /dev/null; fi
	-rm -rf *fifo
