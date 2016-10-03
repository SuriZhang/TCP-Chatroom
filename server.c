//聊天室的服务器端
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

//定义变量记录socket描述符
int sockfd;
//定义socket描述符数组来记录所有客户端的fd
int fds[100];
//定义变量记录数组的下标
int size = 0;

//初始化服务器
void init(void);
//启动服务器
void start(void);
//关闭服务器
void destroy(int signo);
//处理客户端信息
void* rcvMsg(void* p);
//发送消息给所有客户端
void sendMsgToAll(void* msg);

int main()
{
	//设置关闭服务器的信号
	signal(SIGINT,destroy);
	//初始化服务器
	init();
	//启动服务器
	start();
	return 0;
}

//初始化服务器
void init(void)
{
	printf("服务器正在初始化...\n");
	sleep(3);
	//1.创建socket
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == sockfd)
	{
		perror("socket server"),exit(-1);
	}
	//2.准备通信地址
	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8888);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");	
	//3.进行socket和地址的绑定
	int res = bind(sockfd,(struct sockaddr*)&addr,sizeof(addr));
	if(-1 == res)
	{
		perror("bind server"),exit(-1);
	}
	printf("绑定成功\n");
	//4.进行监听
	res = listen(sockfd,100);
	if(-1 == res)
	{
		perror("listen server"),exit(-1);
	}
	printf("服务器初始化成功\n");
}

//启动服务器
void start(void)
{
	printf("服务器正在启动...\n");
	sleep(3);
	printf("服务器启动成功\n");
	while(1)
	{
		//1.准备一个新地址用来接受客户端的地址
		struct sockaddr_in acvAddr;
		socklen_t len = sizeof(acvAddr);
		//2.响应客户端的请求，然后开辟新线程去响应客户端,为了腾出主线程继续响应新的客户端
		int resfd = accept(sockfd,(struct sockaddr*)&acvAddr,&len);
		if(-1 == resfd)
		{
			perror("accept server"),exit(-1);
		}
		printf("阻塞到这里了\n");
		//3.开辟新线程去处理客户端的消息
		pthread_t pid;
		pthread_create(&pid,0,rcvMsg,&resfd);
		//4.记录下连接成功的客户端fd
		fds[size++] = resfd;
	}
}

//关闭服务器
void destroy(int signo)
{
	//6.关闭服务器
	printf("服务器正在关闭...\n");
	close(sockfd);
	sleep(3);
	exit(0);
}

//处理客户端信息
void* rcvMsg(void* p)
{
	int tempFd = *(int*)p;
	while(1)
	{
		//不停地接受来自客户端的消息
		char buf[100] = {};
		if(recv(tempFd,buf,sizeof(buf),0) <= 0)
		{
			//perror("recv server"),exit(-1);
			int i = 0;
			for(i = 0; i < size; i++)
			{
				if(fds[i] == tempFd)
				{
					fds[i] = 0;
				}
			}
		}
		//把接受到的消息发送给所有客户端
		sendMsgToAll(buf);
	}
}

//发送消息给所有客户端
void sendMsgToAll(void* msg)
{
	char* str = (char*)msg;
	int i = 0;
	for(i = 0; i < size; i++)
	{
		if(0 != fds[i])
		{
			send(fds[i],str,strlen(str),0);
		}
	}
}
