#NAME: Wenxuan Liu
#EMAIL: allenliux01@163.com
#ID: 805152602
default: lab1b-server lab1b-client
lab1b-server: lab1b-server.c
	gcc -Wall -Wextra -lz -o lab1b-server lab1b-server.c
lab1b-client: lab1b-client.c
	gcc -Wall -Wextra -lz -o lab1b-client lab1b-client.c
clean:
	rm -f lab1b-client lab1b-server *.o *.tar.gz
dist:
	tar -czvf lab1b-805152602.tar.gz lab1b-server.c lab1b-client.c Makefile README
