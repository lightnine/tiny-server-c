/**
 * tiny server 主程序
 *
 */
#include "csapp.h"
void doit(int fd);
void echo(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);

int main(int argc, char **argv)
{
	int listenfd, connfd;
	// MAXLINE defined in csapp.h 8192
	char hostname[MAXLINE], port[MAXLINE];
	socklen_t clientlen;
	// sockaddr_storage是保存协议及地址的通用结构体,128 byte
	struct sockaddr_storage clientaddr;

	// 检查命令行参数
	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(1);
	}
	// 获取监听的listenfd
	listenfd = Open_listenfd(argv[1]);
	while (1) {
		clientlen = sizeof(clientaddr);
		fprintf(stdin, "clientlen: %d\n", clientlen);
		// clientaddr 转为 SA(sockaddr)
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
		// 将二进制的地址转为字符的hostname和port
		Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
		printf("Accepted connection from (%s, %s)\n", hostname, port);
		echo(connfd);
		//doit(connfd);
		Close(connfd);
	}
}
void echo(int fd) {
	char buf[MAXLINE];
		
	char resp_buf[MAXLINE];
	sprintf(resp_buf, "HTTP/1.0 200 OK\r\n");
	sprintf(resp_buf, "%sServer: Tiny Web Server6\r\n", resp_buf);
	sprintf(resp_buf, "%sConnection: close\r\n", resp_buf);
//	sprintf(resp_buf, "%sContent-length: %d\r\n", resp_buf, (int)strlen(body));
	sprintf(resp_buf, "%sContent-type: text/plain\r\n\r\n", resp_buf);
	Rio_writen(fd, resp_buf, strlen(resp_buf));

	rio_t rio;
	Rio_readinitb(&rio, fd);
	Rio_readlineb(&rio, buf, MAXLINE);
	printf("%s", buf);
	Rio_writen(fd, buf, strlen(buf));
	while(strcmp(buf, "\r\n")) {
		Rio_readlineb(&rio, buf, MAXLINE);
		Rio_writen(fd, buf, strlen(buf));
		printf("%s", buf);
	}
}
/**
 * 处理请求内容
 */
void doit(int fd)
{
	int is_static;
	struct stat sbuf;
	char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
	char filename[MAXLINE], cgiargs[MAXLINE];
	// 缓存结构
	rio_t rio;
	// 初始化rio
	Rio_readinitb(&rio, fd);
	// 读取rio中的一行到buf, 最多是MAXLINE-1个字符
	Rio_readlineb(&rio, buf, MAXLINE);
	printf("Request headers:\n");
	printf("%s", buf);
	// 将buf中的内容读取到method, uri, version中
	sscanf(buf, "%s %s %s", method, uri, version);
	if (strcasecmp(method, "GET")) {
		clienterror(fd, method, "501", "Not implemented", "TIny does not implement this method");
		return;
	}
	read_requesthdrs(&rio);

	is_static = parse_uri(uri, filename, cgiargs);
	// stat将文件信息读取到sbuf中
	if (stat(filename, &sbuf) < 0) {
		clienterror(fd, filename, "404", "Not found", "Tiny couldn't find this file");
		return;
	}
	// st_mode中包含了文件类型, 文件权限等信息
	// S_ISREG判断是否为常规文件
	if (is_static) {
		if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
			clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't read the file");
			return;
		}
		serve_static(fd, filename, sbuf.st_size);
	}
	else {
		if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
			clienterror(fd, filename, "403", "Forbidden", "Tiny could't run the CGI program");
			return;
		}
		serve_dynamic(fd, filename, cgiargs);
	}
}
/**
 * 给客户端返回错误消息
 */
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg) {
	char buf[MAXLINE], body[MAXBUF];
	/* 创建HTTP response body */
	// sprintf将格式化的内容保存在第一个参数中
	sprintf(body, "<html><title>Tiny Error</title>");
	sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
	sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
	sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
	sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

	/*将响应内容写入到fd中 */
	// 响应行
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
	Rio_writen(fd, buf, strlen(buf));
	// 响应头和空行
	sprintf(buf, "Content-type: text/html\r\n");
	Rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
	Rio_writen(fd, buf, strlen(buf));
	// 响应内容
	Rio_writen(fd, body, strlen(body));
}
/**
 * 读取请求头的内容
 */
void read_requesthdrs(rio_t *rp) {
	char buf[MAXLINE];
	// 每次读取一行, 直到遇到空行
	// 简单的打印请求头的内容
	Rio_readlineb(rp, buf, MAXLINE);
	while(strcmp(buf, "\r\n")) {
		Rio_readlineb(rp, buf, MAXLINE);
		printf("%s", buf);
	}
	return;
}
/**
 * 解析uri
 */
int parse_uri(char *uri, char *filename, char *cgiargs) {
	char *ptr;
	// strstr查询cgi-bin在uri中的位置,如果不存在,则返回NULL
	if (!strstr(uri, "cgi-bin")) {
		// 不包含cgi-bin, 静态内容
		strcpy(cgiargs, "");
		strcpy(filename, ".");
		// 将uri增加到filename后
		strcat(filename, uri);
		if (uri[strlen(uri) - 1] == '/') {
			strcat(filename, "home.html");
		}
		return 1;
	}
	else {
		// 查询?在uri中出现的位置
		ptr = index(uri, '?');
		if (ptr) {
			// 将?后面的内容拷贝到cgiargs中
			strcpy(cgiargs, ptr + 1);
			*ptr = '\0';
		} else {
			strcpy(cgiargs, "");
		}
		strcpy(filename, ".");
		strcat(filename, uri);
		return 0;
	}
}

/**
 * 处理静态文件内容
 */
void serve_static(int fd, char *filename, int filesize) {
	int srcfd;
	char *srcp, filetype[MAXLINE], buf[MAXBUF];
	// 获取文件名以及文件类型
	get_filetype(filename, filetype);
	// 响应行
	sprintf(buf, "HTTP/1.0 200 OK\r\n");
	// 响应头
	sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
	sprintf(buf, "%sConnection: close\r\n", buf);
	sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
	sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
	Rio_writen(fd, buf, strlen(buf));
	printf("Response headers:\n");
	printf("%s", buf);

	srcfd = Open(filename, O_RDONLY, 0);
	// 将srcfd文件映射到内存区域中
	srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
	Close(srcfd);
	// 将srcp指定的内存区域写入到fd中
	Rio_writen(fd, srcp, filesize);
	// 将srcp指定的虚拟内存删除
	Munmap(srcp, filesize);
}

/**
 * 从文件后缀来结局定Content-type
 */
void get_filetype(char *filename, char *filetype) {
	if (strstr(filename, ".html")) {
		strcpy(filetype, "text/html");
	} else if (strstr(filename, ".gif")) {
		strcpy(filetype, "image/gif");
	} else if (strstr(filename, ".png")) {
		strcpy(filetype, "image/png");
	} else if (strstr(filename, ".jpg")) {
		strcpy(filetype, "image/jpeg");
	} else {
		strcpy(filetype, "text/plain");
	}
}

void serve_dynamic(int fd, char *filename, char *cgiargs) {
	char buf[MAXLINE], *emptylist[] =  { NULL };

	sprintf(buf, "HTTP/1.0 200 OK\r\n");
	Rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Server: Tiny Web Server\r\n");
	Rio_writen(fd, buf, strlen(buf));

	if (Fork() == 0) {
		// setenv 设置环境变量 QUERY_STRING的值为cgiargs, 并且覆盖写
		setenv("QUERY_STRING", cgiargs, 1);
		// 将标准输出重定向到fd
		Dup2(fd, STDOUT_FILENO);
		// 参数为emtpylist, 环境变量为environ来执行filename文件
		Execve(filename, emptylist, environ);
	}
	Wait(NULL);
}
