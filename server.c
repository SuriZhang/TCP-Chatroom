// Chatroom Server
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

// declare a variable to record socket descriptor
int sockfd;
// declare an array to record all clients' fd
int fds[100];
// variable to keep track of array indices
int size = 0;

// initialize Server
void init(void);
// start Server
void start(void);
// destroy Server
void destroy(int signo);
// receive msg from Client
void* rcvMsg(void* p);
// send msg to all Clients
void sendMsgToAll(void* msg);

int main()
{
	// set signa to destroy Server
	signal(SIGINT,destroy);
	// initialize Server
	init();
	// start Server
	start();
	return 0;
}

// initializing
void init(void)
{
	printf("Server initializing...\n");
	sleep(3);
	//1. create socket
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == sockfd)
	{
		perror("socket server"),exit(-1);
	}
	//2. prepare address
	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8888);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");	
	//3. bind socket to address
	int res = bind(sockfd,(struct sockaddr*)&addr,sizeof(addr));
	if(-1 == res)
	{
		perror("bind server"),exit(-1);
	}
	printf("Binding Suceeded.\n");
	//4. isten
	res = listen(sockfd,100);
	if(-1 == res)
	{
		perror("listen server"),exit(-1);
	}
	printf("Server Initialized.\n");
}

// start Server
void start(void)
{
	printf("Starting Server...\n");
	sleep(3);
	printf("Server Started.\n");
	while(1)
	{
		//1. prepare a new address to accept Client address
		struct sockaddr_in acvAddr;
		socklen_t len = sizeof(acvAddr);
		//2. respond to Client requests and create new thread to reply to 
		// Client and allow the amin thread to respond to the newcoming Cleints
		int resfd = accept(sockfd,(struct sockaddr*)&acvAddr,&len);
		if(-1 == resfd)
		{
			perror("accept server"),exit(-1);
		}
		printf("Error: Stucked here!\n");
		//3. create new thread to respond to Client msg
		pthread_t pid;
		pthread_create(&pid,0,rcvMsg,&resfd);
		//4. record connected Clients' fd
		fds[size++] = resfd;
	}
}

// destroy Server
void destroy(int signo)
{
	//6. destroy Server
	printf("Destorying Server...\n");
	close(sockfd);
	sleep(3);
	exit(0);
}

// receive msg from Client
void* rcvMsg(void* p)
{
	int tempFd = *(int*)p;
	while(1)
	{
		// keep receiving msg from Server
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
		// send received msg to all Clients
		sendMsgToAll(buf);
	}
}

// send msg to all connected Clients
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
