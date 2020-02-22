#NAME: Wenxuan Liu
#EMAIL: allenliux01@163.com
#ID: 805152602
default: build
build: lab2_list.c
	gcc -Wall -Wextra -g -o lab2_list -lpthread -lprofiler lab2_list.c SortedList.c
tests: build
	rm -f lab2b_list.csv
	#plot_1, plot_2 mutex & spin-lock, 1,000 iterations, 1,2,4,8,12,16,24 threads
	./lab2_list --threads=1  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=s >> lab2b_list.csv
	#plot_3 --yield=id, 4 lists, 1,4,8,12,16 threads, and 1, 2, 4, 8, 16 iterations
	-./lab2_list --threads=1  --yield=id --lists=4 --iterations=1  >> lab2b_list.csv
	-./lab2_list --threads=1  --yield=id --lists=4 --iterations=2  >> lab2b_list.csv
	-./lab2_list --threads=1  --yield=id --lists=4 --iterations=4  >> lab2b_list.csv
	-./lab2_list --threads=1  --yield=id --lists=4 --iterations=8  >> lab2b_list.csv
	-./lab2_list --threads=1  --yield=id --lists=4 --iterations=16 >> lab2b_list.csv
	-./lab2_list --threads=4  --yield=id --lists=4 --iterations=1  >> lab2b_list.csv
	-./lab2_list --threads=4  --yield=id --lists=4 --iterations=2  >> lab2b_list.csv
	-./lab2_list --threads=4  --yield=id --lists=4 --iterations=4  >> lab2b_list.csv
	-./lab2_list --threads=4  --yield=id --lists=4 --iterations=8  >> lab2b_list.csv
	-./lab2_list --threads=4  --yield=id --lists=4 --iterations=16 >> lab2b_list.csv
	-./lab2_list --threads=8  --yield=id --lists=4 --iterations=1  >> lab2b_list.csv
	-./lab2_list --threads=8  --yield=id --lists=4 --iterations=2  >> lab2b_list.csv
	-./lab2_list --threads=8  --yield=id --lists=4 --iterations=4  >> lab2b_list.csv
	-./lab2_list --threads=8  --yield=id --lists=4 --iterations=8  >> lab2b_list.csv
	-./lab2_list --threads=8  --yield=id --lists=4 --iterations=16 >> lab2b_list.csv
	-./lab2_list --threads=12 --yield=id --lists=4 --iterations=1  >> lab2b_list.csv
	-./lab2_list --threads=12 --yield=id --lists=4 --iterations=2  >> lab2b_list.csv
	-./lab2_list --threads=12 --yield=id --lists=4 --iterations=4  >> lab2b_list.csv
	-./lab2_list --threads=12 --yield=id --lists=4 --iterations=8  >> lab2b_list.csv
	-./lab2_list --threads=12 --yield=id --lists=4 --iterations=16 >> lab2b_list.csv
	-./lab2_list --threads=16 --yield=id --lists=4 --iterations=1  >> lab2b_list.csv
	-./lab2_list --threads=16 --yield=id --lists=4 --iterations=2  >> lab2b_list.csv
	-./lab2_list --threads=16 --yield=id --lists=4 --iterations=4  >> lab2b_list.csv
	-./lab2_list --threads=16 --yield=id --lists=4 --iterations=8  >> lab2b_list.csv
	-./lab2_list --threads=16 --yield=id --lists=4 --iterations=16 >> lab2b_list.csv
	#plot_3 --yield=id, 4 lists, 1,4,8,12,16 threads, and 10, 20, 40, 80 iterations, --sync=s and --sync=m
	./lab2_list --threads=1  --yield=id --lists=4 --iterations=10 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1  --yield=id --lists=4 --iterations=20 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1  --yield=id --lists=4 --iterations=40 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1  --yield=id --lists=4 --iterations=80 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4  --yield=id --lists=4 --iterations=10 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4  --yield=id --lists=4 --iterations=20 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4  --yield=id --lists=4 --iterations=40 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4  --yield=id --lists=4 --iterations=80 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8  --yield=id --lists=4 --iterations=10 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8  --yield=id --lists=4 --iterations=20 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8  --yield=id --lists=4 --iterations=40 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8  --yield=id --lists=4 --iterations=80 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --yield=id --lists=4 --iterations=10 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --yield=id --lists=4 --iterations=20 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --yield=id --lists=4 --iterations=40 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --yield=id --lists=4 --iterations=80 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --yield=id --lists=4 --iterations=10 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --yield=id --lists=4 --iterations=20 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --yield=id --lists=4 --iterations=40 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --yield=id --lists=4 --iterations=80 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1  --yield=id --lists=4 --iterations=10 --sync=s >> lab2b_list.csv
	./lab2_list --threads=1  --yield=id --lists=4 --iterations=20 --sync=s >> lab2b_list.csv
	./lab2_list --threads=1  --yield=id --lists=4 --iterations=40 --sync=s >> lab2b_list.csv
	./lab2_list --threads=1  --yield=id --lists=4 --iterations=80 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4  --yield=id --lists=4 --iterations=10 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4  --yield=id --lists=4 --iterations=20 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4  --yield=id --lists=4 --iterations=40 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4  --yield=id --lists=4 --iterations=80 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8  --yield=id --lists=4 --iterations=10 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8  --yield=id --lists=4 --iterations=20 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8  --yield=id --lists=4 --iterations=40 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8  --yield=id --lists=4 --iterations=80 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --yield=id --lists=4 --iterations=10 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --yield=id --lists=4 --iterations=20 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --yield=id --lists=4 --iterations=40 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --yield=id --lists=4 --iterations=80 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --yield=id --lists=4 --iterations=10 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --yield=id --lists=4 --iterations=20 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --yield=id --lists=4 --iterations=40 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --yield=id --lists=4 --iterations=80 --sync=s >> lab2b_list.csv
	#plot_4 plot_5 both synchronized versions, without yields, for 1000 iterations, 1,2,4,8,12 threads, and 1,4,8,16 lists.
	./lab2_list --threads=1  --iterations=1000 --sync=m	--lists=4  >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=m	--lists=4  >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=m	--lists=4  >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=m	--lists=4  >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m --lists=4  >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=s	--lists=4  >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=s --lists=4  >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=s --lists=4  >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=s --lists=4  >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s --lists=4  >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=m --lists=8  >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=m	--lists=8  >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=m --lists=8  >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=m --lists=8  >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m --lists=8  >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=s	--lists=8  >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=s --lists=8  >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=s --lists=8  >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=s --lists=8  >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s --lists=8  >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=m	--lists=16 >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=m	--lists=16 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m --lists=16 >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=s	--lists=16 >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s --lists=16 >> lab2b_list.csv

graphs: tests
	./lab2b_list.gp

profile: default
	rm -f ./raw.gperf profile.out
	LD_PRELOAD=/usr/local/cs/gperftools-2.7/lib/libprofiler.so
	CPUPROFILE=./raw.gperf ./lab2_list --threads=12 --iterations=1000 --sync=s
	pprof --text ./lab2_list ./raw.gperf > profile.out
	pprof --list=threadFunc ./lab2_list ./raw.gperf >> profile.out
	rm -f ./raw.gperf profile.gperf lab2_list

clean:
	rm -f lab2_list *.tar.gz
dist: graphs
	tar -cvzf lab2b-805152602.tar.gz lab2_list.c SortedList.h SortedList.c lab2b_list.csv \
	README Makefile lab2b_list.gp profile.out \
	lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png
