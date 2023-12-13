CC=g++
CFLAGS=-fPIC -Wall
LIBNAME=libbitflip.so
TESTNAME=testbitflip

all: $(LIBNAME) $(TESTNAME)

$(LIBNAME): bitflip.cc bitflip.h
    $(CC) $(CFLAGS) -shared $< -o $@

$(TESTNAME): bitflip.cc bitflip.h
    $(CC) $(CFLAGS) $< -o $@

clean:
    rm -f $(LIBNAME) $(TESTNAME)
