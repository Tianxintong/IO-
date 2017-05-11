#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
  #include <libgen.h>
#include<stdlib.h>
int main(int argc,char *argv[])
{
	if(argc<=2)
	{
		printf("usage:%s  ip   port\n",basename(argv[0]));
		return 1;
	}
	const char *ip = argv[1];
	int port = atoi(argv[2]);

	struct sockaddr_in address;
	bzero(&address,sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	inet_pton(AF_INET,ip,&address.sin_addr);
	
	int sock = socket(AF_INET,SOCK_STREAM,0);
	assert(sock >= 0);

	int ret = bind(sock,(struct sockaddr*)&address,sizeof(address));
	assert(ret != -1);

	ret = listen(sock,5);
	assert(ret!=-1);

	struct sockaddr_in client;
	socklen_t client_len = sizeof(client);
	int connfd = accept(sock,(struct sockaddr*)&client,&client_len);

	if(connfd < 0)
	{
		printf("errno is %d\n",errno);
	}
	else
	{
		char buf[50];
			close(STDOUT_FILENO);
			dup(connfd);
		while(1)
		{
			scanf("%s",buf);
			printf("%s",buf);
			send(connfd, buf,strlen(buf)+1,0);
			memset(buf,'\0',50);
		}
		close(connfd);
	}
	
}
