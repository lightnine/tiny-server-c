在echo中，展示了使用c语言如何将客户端的请求发送到服务端。这里完全没有用到并发，每次客户端只能处理一个请求。

# 编译

```shell
cd echo
// 编译服务器端代码，其中-I表示指定头文件所在的目录，-lpthread表示链接pthread库
// 注意这里需要将echo.c写入, 因为echoserver.c依赖echo.c，所以需要将echo.c编译并链接到echoserver的可执行文件中
gcc -arch arm64 -I .. echoserver.c echo.c ../csapp.c -o echoserver -lpthread
// 编译客户端代码
gcc -arch arm64 -I .. echoclient.c ../csapp.c -o echoclient -lpthread
```

# 运行
```shell
// 服务端
./echoserver 8080
// 客户端
./echoclient 127.0.0.1 8080
```
然后在客户端输入文本，可以看到服务端收到请求，客户端收到服务端的响应。