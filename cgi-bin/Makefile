CC = gcc
# 采用O2的优化级别,Wall生成所有警告信息
# I 参数指定在上一级中搜索头文件
CFLAGS = -O2 -Wall -I ..

all: adder

adder: adder.c
	$(CC) $(CFLAGS) -o adder adder.c

clean:
	rm -f adder *~
