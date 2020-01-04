#include "../csapp.h"

int main(void) {
	char *buf, *p;
	char arg1[MAXLINE], arg2[MAXLINE], content[MAXLINE];
	int n1=0, n2=0;
	/* 读取环境变量QUERY_STRING中的内容,分别保存在arg1和arg2中,并且转为整形*/
	if ((buf = getenv("QUERY_STRING")) != NULL) {
		p = strchr(buf, '&');
		*p = '\0';
		strcpy(arg1, buf);
		strcpy(arg2, p+1);
		n1 = atoi(arg1);
		n2 = atoi(arg2);
	}
	// 将输出内容保存到content中
	sprintf(content, "QUERY_STRING=%s", buf);
	sprintf(content, "Welcome to add.com:");
	sprintf(content, "%sTHE Internet addition portal.\r\n<p>", content);
	sprintf(content, "%sThe answer is: %d + %d = %d\r\n<p>", content, n1 , n2, n1 + n2);
	sprintf(content, "%sThanks for visiting!\r\n", content);
	// 将内容输出到标准输出,主程序中会将标准输出重定向到对应的连接描述符
	printf("Connection: close\r\n");
	printf("Content-length: %d\r\n", (int)strlen(content));
	printf("Content-type: text/html\r\n\r\n");
	printf("%s", content);
	fflush(stdout);

	exit(0);
}
