all: driver

driver: driver.o liblog.a
	gcc -lm -o driver driver.o -L. -llog

driver.o: driver.c
	gcc -O -c driver.c

liblog.o: liblog.c liblog.h
	gcc -O -c liblog.c

liblog.a: liblog.o
	ar rcs liblog.a liblog.o

libs: liblog.a

clean:
	rm -f driver *.o *.a *.log