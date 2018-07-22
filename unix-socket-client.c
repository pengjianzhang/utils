#include <stdio.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>



int unix_socket_connect(const char *name)
{
    int fd, len, err, rval;
    struct sockaddr_un un;

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        return(-1);  

    if(fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
        close(fd);
        return -1;
    }

    memset(&un, 0, sizeof(un));
    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, name);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(name);


    if ((err = connect(fd, (struct sockaddr *)&un, len)) != 0)
    {
        if(errno != EINPROGRESS)
        {
            close(fd);
            return -1;
        }
    }  else {
        printf("connect ok\n");
    }

    return fd;
}


int unix_socket_send(int fd, char * msg)
{
    int len = strlen(msg);
    int size;

    size = send(fd,msg,len,0);

    printf("send %d\n",size);
    return size;
}


int unix_socket_recv(int fd, char * buf, int len)
{
    int size = 0;

    size = recv(fd,buf,len - 1,0);
    if(size >= 0) {
        buf[size] = 0; 
        printf("%s\n",buf);     
    }

    return size;
}

char * req2 =    "GET / HTTP/1.0\r\n"
                "Connection: close\r\n"
                "User-Agent: unix-socket\r\n"
                "Accept: */*\r\n"
                "\r\n";


char * req =    "GET / HTTP/1.0\r\n\r\n";


char g_buf[1024];

int main(int argc, char *argv[])
{
    int fd;

    if(argc != 2) {
        return 1;    
    }    
    
    
    fd = unix_socket_connect(argv[1]);

    if(fd < 0) {
        return 1;    
    }

    unix_socket_send(fd, req);
    usleep(1000);
    unix_socket_recv(fd, g_buf, 1024);

    close(fd);

    return 0;
}
