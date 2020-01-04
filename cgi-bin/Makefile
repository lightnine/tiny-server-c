CC = gcc
# Wall生成所有警告信息
CFLAGS = -m64 -Wall -pthread

all: 
	$(CC) $(CFLAGS) adder.c ../csapp.c -o adder

.PHONY: clean
clean:
	@# find -type指定查询块设备,目录,字符设备,管道,符号链接,普通文件,后面-f参数表示查询普通文件
	@# -executable对于查找的文件执行操作, 后面的-print0打印文件,加上0不会添加回车换行符
	@# xargs是给其他命令传递参数的一个过滤器, rm后面的--表示对于以"-"开始的文件以删除
	find . -type -f -executable -print0 | xargs -0 rm -f --
