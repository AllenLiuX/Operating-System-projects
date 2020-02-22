#NAME: Wenxuan Liu
#EMAIL: allenliux01@163.com
#ID: 805152602
default: build
build: lab2_add.c lab2_list.c
	gcc -Wall -Wextra -g -o lab2_list -lpthread lab2_list.c SortedList.c
	gcc -Wall -Wextra -g -o lab2_add -lpthread lab2_add.c
tests: build
	rm -f lab2_add.csv lab2_list.csv
	#plot_1
	./lab2_add --threads=2  --iterations=10     --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10     --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10     --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10     --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=20     --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=20     --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=20     --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=20     --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=40     --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=40     --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=40     --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=40     --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=80     --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=80     --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=80     --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=80     --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100    --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100    --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100    --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=100    --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=1000   --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=1000   --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=1000   --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=1000   --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000  --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000  --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000  --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000  --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100000 --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100000 --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100000 --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=100000 --yield >> lab2_add.csv

	#plot_2, 2.1.2, plot_3, 2.1.3
	./lab2_add --threads=1  --iterations=10      >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100     >> lab2_add.csv
	./lab2_add --threads=1  --iterations=1000    >> lab2_add.csv
	./lab2_add --threads=1  --iterations=10000   >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100000  >> lab2_add.csv
	./lab2_add --threads=1  --iterations=1000000 >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100     >> lab2_add.csv
	./lab2_add --threads=2  --iterations=1000    >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000   >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100000  >> lab2_add.csv
	./lab2_add --threads=2  --iterations=1000000 >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100     >> lab2_add.csv
	./lab2_add --threads=4  --iterations=1000    >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000   >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100000  >> lab2_add.csv
	./lab2_add --threads=4  --iterations=1000000 >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100     >> lab2_add.csv
	./lab2_add --threads=8  --iterations=1000    >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000   >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100000  >> lab2_add.csv
	./lab2_add --threads=8  --iterations=1000000 >> lab2_add.csv
	./lab2_add --threads=12 --iterations=100     >> lab2_add.csv
	./lab2_add --threads=12 --iterations=1000    >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000   >> lab2_add.csv
	./lab2_add --threads=12 --iterations=100000  >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100     --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=1000    --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=10000   --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100000  --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=1000000 --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=1000000 --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=1000000 --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=1000000 --yield >> lab2_add.csv

	#plot_4
	./lab2_add --threads=2  --iterations=10000 --yield --sync=m >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 --yield --sync=m >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000 --yield --sync=m >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 --yield --sync=m >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000 --yield --sync=c >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 --yield --sync=c >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000 --yield --sync=c >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 --yield --sync=c >> lab2_add.csv
	./lab2_add --threads=2  --iterations=1000  --yield --sync=s >> lab2_add.csv
	./lab2_add --threads=4  --iterations=1000  --yield --sync=s >> lab2_add.csv
	./lab2_add --threads=8  --iterations=1000  --yield --sync=s >> lab2_add.csv
	./lab2_add --threads=12 --iterations=1000  --yield --sync=s >> lab2_add.csv

	#plot_5
	./lab2_add --threads=1  --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=1  --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=1  --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 --sync=s >> lab2_add.csv
	#Part II
	#plot_1	
	./lab2_list --threads=1  --iterations=10	>> lab2_list.csv
	./lab2_list --threads=1  --iterations=100	>> lab2_list.csv
	./lab2_list --threads=1  --iterations=1000	>> lab2_list.csv
	./lab2_list --threads=1  --iterations=10000	>> lab2_list.csv
	./lab2_list --threads=1  --iterations=20000	>> lab2_list.csv
	#parallel threads (2,4,8,12) and iterations (1, 10,100,1000)
	-./lab2_list --threads=2  --iterations=1    >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=10   >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=100  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=1000 >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1    >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=10   >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=100  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1000 >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1    >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=10   >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=100  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1000 >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1    >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=10   >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=100  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1000 >> lab2_list.csv
	#plot_2, threads (2,4,8,12) and iterations (1, 2,4,8,16,32) -i
	-./lab2_list --threads=2  --iterations=1   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=2   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=4   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=8   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=16  --yield=i  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=32  --yield=i  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=2   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=4   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=8   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=16  --yield=i  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=32  --yield=i  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=2   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=4   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=8   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=16  --yield=i  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=32  --yield=i  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16  --yield=i  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=32  --yield=i  >> lab2_list.csv
	#-d
	-./lab2_list --threads=2  --iterations=1   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=2   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=4   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=8   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=16  --yield=d  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=32  --yield=d  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=2   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=4   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=8   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=16  --yield=d  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=32  --yield=d  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=2   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=4   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=8   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=16  --yield=d  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=32  --yield=d  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16  --yield=d  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=32  --yield=d  >> lab2_list.csv
	#-il
	-./lab2_list --threads=2  --iterations=1   --yield=il  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=2   --yield=il  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=4   --yield=il  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=8   --yield=il  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=16  --yield=il  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=32  --yield=il  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1   --yield=il  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=2   --yield=il  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=4   --yield=il  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=8   --yield=il  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=16  --yield=il  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=32  --yield=il  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1   --yield=il  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=2   --yield=il  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=4   --yield=il  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=8   --yield=il  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=16  --yield=il  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=32  --yield=il  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1   --yield=il  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2   --yield=il  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4   --yield=il  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8   --yield=il  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16  --yield=il  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=32  --yield=il  >> lab2_list.csv
	#-dl
	-./lab2_list --threads=2  --iterations=1   --yield=dl  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=2   --yield=dl  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=4   --yield=dl  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=8   --yield=dl  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=16  --yield=dl  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=32  --yield=dl  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1   --yield=dl  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=2   --yield=dl  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=4   --yield=dl  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=8   --yield=dl  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=16  --yield=dl  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=32  --yield=dl  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1   --yield=dl  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=2   --yield=dl  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=4   --yield=dl  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=8   --yield=dl  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=16  --yield=dl  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=32  --yield=dl  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1   --yield=dl  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2   --yield=dl  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4   --yield=dl  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8   --yield=dl  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16  --yield=dl  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=32  --yield=dl  >> lab2_list.csv
	#plot_3, eliminate the problem for moderate threads(12) and iterations(32)
	./lab2_list --threads=12 --iterations=32 --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=dl --sync=s >> lab2_list.csv

	#plot_4, without yield, threads (1, 2, 4, 8, 12, 16, 24)
	./lab2_list --threads=1  --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=s >> lab2_list.csv
graphs: tests
	./lab2_add.gp
	./lab2_list.gp

clean:
	rm -f lab2_add lab2_list *.tar.gz
dist: graphs
	tar -cvzf lab2a-805152602.tar.gz lab2_add.c lab2_list.c SortedList.h SortedList.c lab2_add.csv lab2_list.csv \
	README Makefile lab2_add.gp lab2_list.gp \
	lab2_add-1.png lab2_add-2.png lab2_add-3.png lab2_add-4.png lab2_add-5.png \
	lab2_list-1.png lab2_list-2.png lab2_list-3.png lab2_list-4.png
