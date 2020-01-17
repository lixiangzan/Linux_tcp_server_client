#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ctype.h>
 
#define SERVER_PORT 80 //监听本机8000端口
#define MAX 4096
int main(void) 
{
	struct sockaddr_in serveraddr,clientaddr;
	int sockfd,addrlen,confd,len;
	char ipstr[128];
	char buf[4096];
	pid_t pid;
	//1.socket
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	//2.bind
	bzero(&serveraddr,sizeof(serveraddr));
	//地址族协议ipv4
	serveraddr.sin_family = AF_INET;
	//ip地址
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVER_PORT);
	bind(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
	//3.listen
	listen(sockfd,128);
	while(1){
		//4. accept阻塞监听客户端的链接请求
		addrlen = sizeof(clientaddr);
		confd = accept(sockfd,(struct sockaddr *)&clientaddr,&addrlen);
		//如果有客户端连接上服务器，就输出客户端的ip地址和端口号
		printf("client ip %s\tport %d\n",
		inet_ntop(AF_INET,(struct sockaddr *)&clientaddr.sin_addr.s_addr,ipstr,sizeof(ipstr)),ntohs(clientaddr.sin_port));
		//这块是多进程的关键，当accept收到了客户端的连接之后，就创建子进程,让子进程去处理客户端
                //发来的数据，父进程里面关闭confd（因为用不到了），然后父进程回到while循环继续监听客户端的连接
                pid = fork();
                //5. 子进程处理客户端请求
		if(pid == 0){//子进程
			close(sockfd);
			while(1){//循环读取客户端发来的数据，把小写变成大写
				len = read(confd,buf,sizeof(buf));
				int i = 0;
				while(i < len){
			           	buf[i] = toupper(buf[i]);
			           	i++;
				}
				write(confd,buf,len);
				}
			 	close(confd);
				return 0;	
		    }
		else if(pid > 0){//父进程关闭文件描述符，释放资源
			close(confd);
            }
        }
	return 0;
}

