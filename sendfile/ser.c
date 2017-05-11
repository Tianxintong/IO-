#include"../utili.h"

int main(int argc,char *argv[])
{
	if(argc<=3)
	{
		printf("usage: %s  ip  port_number\n",basename(argv[0]));
		return 1;
	}
	const char*ip = argv[1];
	int port = atoi(argv[2]);
	const char *file_name = argv[3];

	int filefd = open(file_name,O_RDONLY);
	assert(filefd>0);
	struct stat stat_buf;
	fstat(filefd,&stat_buf);

	struct sockaddr_in address;
	bzero(&address,sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	inet_pton(AF_INET,ip,&address.sin_addr);

	int sock = socket(AF_INET,SOCK_STREAM,0);
	assert(sock>=0);
	socklen_t len = sizeof(address);
	int ret = bind(sock,(struct sockaddr*)&address,len);
	assert(ret!=-1);

	ret = listen(sock,5);
	assert(ret!=-1);

	struct sockaddr_in client;
	socklen_t client_len = sizeof(client);
	int connfd = accept(sock,(struct sockaddr*)&client,&client_len);
	if(connfd<0)
	{
		printf("errno: %d\n",errno);
	}
	else
	{
		sendfile(connfd,filefd,NULL,stat_buf.st_size);
		close(connfd);
	}
close(sock);
return 0;
}
