# Makefile prototype for configure
# Copyright 2004 Phil Karn, KA9Q
# May be used under the terms of the GNU Lesser General Public License (LGPL)

CC=gcc

CFLAGS=-g -O2 -I. -Wall

all: rstest.elf

rstest.elf: rstest.o encode_rs.o decode_rs.o init_rs.o
	gcc $(CFLAGS) -o $@ $^

encode_rs.o: encode_rs.c rs.h
	gcc $(CFLAGS) -c $<

decode_rs.o: decode_rs.c rs.h
	gcc $(CFLAGS) -c $<

init_rs.o: init_rs.c rs.h
	gcc $(CFLAGS) -c $<


clean:
	rm -f *.o rstest.elf

