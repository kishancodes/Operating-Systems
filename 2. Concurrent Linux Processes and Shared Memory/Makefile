runsimFile = runsim
testsimFile = testsim
licenseobjFile = licenseobj
OBJS1 = runsim.o licenseobj.o
OBJS2 = testsim.o licenseobj.o
OBJS3 = sharedmem.o

# dependency files, auto generated from source files
DEPS = $(patsubst %.o,%.h,$(obj))
-include $(DEPS)
DEPFLAGS = -MMD -MF $(@:.o=.h)

# the compiler to use
CC = gcc

# compiler flags:
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -Wall


all: $(runsimFile) $(testsimFile) $(licenseobjFile)

$(runsimFile): $(OBJS1)
        $(CC) $(CFLAGS) -o $@ $^

$(testsimFile): $(OBJS2)
        $(CC) $(CFLAGS) -o $@ $^

$(licenseobjFile): $(OBJS2)
        $(CC) $(CFLAGS) -o $@ $^

clean:
        rm -f $(runsimFile) $(testsimFile) $(OBJS1) $(OBJS2) $(OBJS3) $(DEPS)

help:
        @echo available targets: all clean