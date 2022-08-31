#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define SERVER_PORT 8888
#define BUFF_LEN 1024

void handle_udp_msg(int fd)
{
    char buf[BUFF_LEN];    //Receive buffer
    socklen_t len;
    int count;
    struct sockaddr_in client_addr;  //Record the address of the sender

    while (1)
    {
       memset(buf,0,BUFF_LEN);
       len = sizeof(client_addr);
       count = recvfrom(fd,buf,BUFF_LEN,0,(struct sockaddr*)&client_addr,&len);
       if(count == -1)
       {
            printf("recieve data fail!\n");
            return ;

       }

       printf("client :%s\n",buf);
       memset(buf,0,BUFF_LEN);
       sprintf(buf,"I have recieved %d bytes data! \n",count);
       sendto(fd,buf,BUFF_LEN,0,(struct sockaddr*)&client_addr,len);
    }
    
}


int main(int argc,char* argv[])
{
    int server_fd,ret;
    struct  sockaddr_in ser_addr;
    
    server_fd = socket(AF_INET,SOCK_STREAM,0);
    if(server_fd<0)
    {
        printf("create socket fail!\n");
        return -1;
    }
    printf("create socket Yes!\n");

    memset(&ser_addr,0,sizeof(ser_addr));//Copy to cover
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    ser_addr.sin_port= htons(SERVER_PORT);

    ret = bind(server_fd,(struct sockaddr*)&ser_addr,sizeof(ser_addr));
    if(ret<0)
    {
        printf("socket bind fail!\n");
        return -1;
    }

    handle_udp_msg(server_fd);

    //close(server_fd);
    return 0;
}
 