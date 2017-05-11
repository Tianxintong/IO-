#include"../utili.h"
#include<linux/stat.h>
#define BUFFER_SIZE 1024
static const char *status_line[2] = {"200  OK","500 INternal server error"};

int main(int argc,char *argv[])
{
	if(argc<=3)
	{
		printf("usage:%s  ip  port_number  filename\n",basename(argv[0]));
		return 1;
	}
	const char* ip = argv[1];
	int port = atoi(argv[2]);
	const char *file_name = argv[3];

	struct sockaddr_in address;
	bzero(&address,sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	inet_pton(AF_INET,ip,&address.sin_addr);

	int sock = socket(AF_INET,SOCK_STREAM,0);
	assert(sock>=0);

	int ret = bind(sock,(struct sockaddr*)&address,sizeof(address));
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
		char head_buffer[BUFFER_SIZE];
		memset(head_buffer,'\0',BUFFER_SIZE);
		char *file_buf;
		struct stat file_stat;
		int vaild = 1;
		int len = 0;
		if(stat(file_name,&file_stat)<0)
		{
			vaild = 0;
		}
		else
		{
			if(S_ISDIR(file_stat.st_mode))
			{
				vaild = 0;
			}
			else if(file_stat.st_mode &S_IROTH)
			{
				int fd = open(file_name,O_RDONLY);
				file_buf = (char*)malloc(file_stat.st_size+1);
				memset(file_buf,'\0',file_stat.st_size+1);
				if(read(fd,file_buf,file_stat.st_size)<0)
				{
					vaild = 0;
				}
			}
			else
			{
				vaild = 0;
			}
		}
		if(vaild)
		{
			ret = snprintf(head_buffer,BUFFER_SIZE-1,"%s %s\r\n","HTTP/1.1",status_line[0]);
			len += ret;
			ret = snprintf(head_buffer+len,BUFFER_SIZE-1-len,"Content-Length:%d\r\n",file_stat.st_size);
			len +=ret;
			ret = snprintf(head_buffer+len,BUFFER_SIZE-1-len,"%s","\r\n");
			struct iovec iv[2];
			iv[0].iov_base = head_buffer;
			iv[0].iov_len = strlen(head_buffer);
			iv[1].iov_base = file_buf;
			iv[1].iov_len = file_stat.st_size;
			ret = writev(connfd,iv,2);

		}
		else
		{
			ret = snprintf(head_buffer,BUFFER_SIZE-1,"%s %s\r\n","HTTP/1.1",status_line[1]);
			len +=ret;
			ret = snprintf(head_buffer+len,BUFFER_SIZE-1-len,"%s","\r\n");
			send(connfd,head_buffer,strlen(head_buffer),0);

		}
		close(connfd);
		free(file_buf);
	}
	close(sock);
	return 0;
}
