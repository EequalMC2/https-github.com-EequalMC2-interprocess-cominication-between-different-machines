#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define BUFFER_SIZE 1024

void exit_sys(const char* msg);

int main(int argc, char** argv)
{

    int sock, server_sock, rv;
    struct sockaddr_in sinaddr;

    if (argc != 3) {
        printf("usage : client <ip> <port>\n");
        exit(EXIT_FAILURE);
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        exit_sys("socket");
    sinaddr.sin_family = AF_INET;
    sinaddr.sin_port = htons(strtol(argv[2], NULL, 10));
    //sinaddr.sin_addr.s_addr = htonl(127 << 24 | 0 << 16 | 0 << 8 | 1);
    rv = inet_aton(argv[1], &sinaddr.sin_addr);
    if (rv == 0) {
        struct hostent* hent;
        hent = gethostbyname(argv[1]);
        if (hent == NULL)
            exit_sys("gethostbyname");
        memcpy(&sinaddr.sin_addr.s_addr, hent->h_addr_list[0], hent->h_length);
    }
    rv = connect(sock, (struct sockaddr*)&sinaddr, sizeof(sinaddr));
    if (rv == -1)
        exit_sys("connect");
    //send data to server
    char buf[BUFFER_SIZE + 1];
    ssize_t response;
    char* str;
    for (;;) {
        printf("please write someting :");
        fgets(buf, BUFFER_SIZE, stdin);
        if ((str = strchr(buf, '\n')) != NULL) {
            *str = '\0';
        }
        response = send(sock, buf, strlen(buf), 0);
        if (response == -1)
            exit_sys("send");

        if (!strcmp(buf, "exit"))
            break;
        
        response = recv(sock, buf, BUFFER_SIZE, 0);
        if(response == -1)
            exit_sys("recv");

        if(response == 0)
            break;

        buf[response];
        printf("getting message : %s\n",buf);
    }
    shutdown(sock, SHUT_RDWR);
    close(sock);
}
void exit_sys(const char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
