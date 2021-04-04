#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#define SERVER_PORT 5000
#define SERVER_IP "127.0.0.1"
void exit_sys(const char* msg);

int main()
{
    int sock, server_sock, rv;
    struct sockaddr_in sinaddr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        exit_sys("socket");

    sinaddr.sin_family = AF_INET;
    sinaddr.sin_port = htons(SERVER_PORT);
    //sinaddr.sin_addr.s_addr = htonl(127 << 24 | 0 << 16 | 0 << 8 | 1);
    rv = inet_aton(SERVER_IP, &sinaddr.sin_addr);
    if (rv == INADDR_NONE) {
        /*DNS OPERATION*/
        exit_sys("inet_aton");
    }

    rv = connect(sock, (struct sockaddr*)&sinaddr, sizeof(sinaddr));
    if (rv == -1)
        exit_sys("connect");


}

void exit_sys(const char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}