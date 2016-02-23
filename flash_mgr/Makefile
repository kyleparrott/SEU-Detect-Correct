# Makefile prototype for configure
# Copyright 2004 Phil Karn, KA9Q
# May be used under the terms of the GNU Lesser General Public License (LGPL)

CC=gcc

CFLAGS=-g -O2 -I. -Wall

all: flash_mgr.elf

rstest.elf: flash_mgr.o
	gcc $(CFLAGS) -o $@ $^

flash_mgr.o: flash_mgr.h
	gcc $(CFLAGS) -c $<

clean:
	rm -f *.o flash_mgr.elf

