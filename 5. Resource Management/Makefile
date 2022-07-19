AR=ar
CC=gcc
CFLAGS=-Wall -std=gnu99
LDFLAGS=

EXECUTABLES=user oss
LIBRARIES=clock.a resmgr.a

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.a: %.o
	$(AR) rcs $@ $<

%.part: %.o $(LIBRARIES)
	$(CC) -o $@ $^ $(LDFLAGS)

all: user.part oss.part
	mv user.part user
	mv oss.part oss

clean:
	rm *.a *.o $(EXECUTABLES)

.PHONY: clean
.SECONDARY:
