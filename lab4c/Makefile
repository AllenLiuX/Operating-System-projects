default:
	gcc -lmraa -lm -Wall -Wextra -g -o lab4c_tcp lab4c_tcp.c
	gcc -lmraa -lm -lssl -lcrypto -Wall -Wextra -g -o lab4c_tls lab4c_tls.c

clean:
	rm -f lab4c_tcp lab4c_tls *.o *.tar.gz

dist:
	tar -czvf lab4c-805152602.tar.gz README Makefile lab4c_tcp.c lab4c_tls.c