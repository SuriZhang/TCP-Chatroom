//Chatroom Client
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

// declare socket 
int sockfd;

// intialize Client
void init(void);
// start Client and send message to Server
void start(void);
// Destroy Server
void destroy(int signo);
// Receive Msg
void* rcvMsg(void* p);

int main()
{
	// set signal to stop Client operations
	signal(SIGINT,destroy);
	// initialization
	init();
	// start Server
	start();
	return 0;
}

// Initialization
void init(void)
{
	printf("Connecting to Server...\n");
	sleep(3);
	//1. create socket
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == sockfd)
	{
		perror("socket"),exit(-1);
	}
	//2. prepare socket address
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8888);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//3. connect to Server
	int res = connect(sockfd,(struct sockaddr*)&addr,sizeof(addr));
	if(-1 == res)
	{
		perror("connect client"),exit(-1);
	}
	printf("Server Connected.\n");
}

// Start Client to send msg to Server 
void start(void)
{
	//1. Send msg to Server
	printf("Enter your username:\n");
	char name[100] = {};
	scanf("%s",name);
	strcat(name,":"); 
	//2. create new thread to receive data from Server
	pthread_t pid;
	pthread_create(&pid,0,rcvMsg,0);
	while(1)
	{
		char buf[100] = {};
		sleep(3);
		printf("Enter your content:\n");
		scanf("%s",buf);
		char msg[200] = {};
		sprintf(msg,"%s%s",name,buf);;
		// Send data
		send(sockfd,msg,strlen(msg),0);
	}
}

// Destroy Client
void destroy(int signo)
{
	printf("Client Destroying...\n");
	close(sockfd);
	sleep(3);
	exit(0);
}

// Receive Msg from Server
void* rcvMsg(void* p)
{
	while(1)
	{
		char buf[200] = {};
		recv(sockfd,buf,sizeof(buf),0);
		printf("%s\n",buf);
	}
}
