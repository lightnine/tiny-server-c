CC = gcc
CFLAGS = -O2 -Wall -I .

# This flag includes the Pthreads library on a Linux box.
# # Others systems will probably require something different.
LIB = -lpthread

# 冒号前是目标,冒号后是前置条件
all: tiny cgi

tiny: tiny.c csapp.o
	@# -o 指定编译文件的结果文件名称
	$(CC) $(CFLAGS) -o tiny tiny.c csapp.o $(LIB)

csapp.o: csapp.c
	@# -c参数只激活预处理,编译和汇编
	$(CC) ${CLAGS} -c csapp.c

cgi: 
	(cd cgi-bin; make)

.PHONY : clean
	clean:
	rm -f *.o tiny *~
	(cd cgi-bin; make clean)
