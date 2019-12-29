## makefile
#
CC = gcc
CFLAGS = -m64 -pthread
SRCS = $(wildcard 8.*.c)
PROGS = $(patsubst %.c,%,$(SRCS))

all: $(PROGS)
		$(CC) $(CFLAGS) -o tiny tiny.c csapp.c
%: %.c
		$(CC) $(CFLAGS) -o $@ $< csapp.c

.PHONY : clean
	clean: rm -f $(PROGS) *.o
