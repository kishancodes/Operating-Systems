all: runsim testsim

testsim: testsim.o license.o
	gcc -pthread -g -o testsim testsim.o license.o

runsim: runsim.o license.o
	gcc -pthread -g -o runsim runsim.o license.o

runsim.o: runsim.c
	gcc -pthread -g -c runsim.c

testsim.o: testsim.c
	gcc -pthread -g -c testsim.c

license.o: license.c
	gcc -pthread -g -c license.c

clean:
	rm -rf *.o runsim testsim
