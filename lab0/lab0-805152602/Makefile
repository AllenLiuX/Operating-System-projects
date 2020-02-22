#NAME: Wenxuan Liu
#EMAIL: allenliux01@163.com
#ID: 805152602

CC=gcc
all: lab0
lab0: lab0.c
	gcc -Wall -Wextra -g -o lab0 lab0.c
clean:
	rm lab0 *.gz
check: lab0 smoketest.sh
	./smoketest.sh
dist:
	tar -cvzf lab0-805152602.tar.gz lab0.c Makefile smoketest.sh README backtrace.png breakpoint.png
