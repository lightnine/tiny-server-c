# makefile
#
CC = gcc
CFLAGS = -m64 -Wall -pthread
# 在当前目录获取所有的tiny.*.c的文件
SRCS = $(wildcard tiny.*.c)
# 将SRCS中的所有文件已.c结尾的去除.c
PROGS = $(patsubst %.c, %, $(SRCS))

all: $(PROGS)
	(cd cgi-bin; make)

%: %.c
	@# $@指代当前目标, $<指代第一个前置条件,在这里就是要编译的.c文件
	$(CC) $(CFLAGS) -o $@ $< csapp.c

cgi: 
	(cd cgi-bin; make)

.PHONY : clean
	clean:
	(cd cgi-bin; make clean)
	find . -type f -executable -print0 | xargs -0 rm -f --
