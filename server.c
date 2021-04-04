#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#define SERVER_PORT 5000

void exit_sys(const char* msg);

int main()
{
    int listen_sock, client_sock, rv;
    struct sockaddr_in server_sinaddr, client_sinaddr;

    //server side network configuration
    server_sinaddr.sin_family = AF_INET;
    server_sinaddr.sin_port = htons(SERVER_PORT);
    server_sinaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_sock == -1)
        exit_sys("socket ");

    rv = bind(listen_sock, (struct sockaddr*)&server_sinaddr, sizeof(server_sinaddr));
    if (rv == -1)
        exit_sys("bind");

    rv = listen(listen_sock, 8);
    if (rv == -1)
        exit_sys("listen");

    printf("waiting for connection....\n");

    socklen_t client_sinaddr_len = sizeof(client_sinaddr);
    client_sock = accept(listen_sock, (struct sockaddr*)&client_sinaddr, &client_sinaddr_len);
    if (client_sock == -1)
        exit_sys("accept");

    printf("Connected %s  : %u\n", inet_ntoa(client_sinaddr.sin_addr), ntohs((unsigned)client_sinaddr.sin_port));

    /*other side*/
    return 0;
}

void exit_sys(const char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}