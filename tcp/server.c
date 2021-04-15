
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define SERVER_PORT 5050
#define BUFFER_SIZE 1024
void exit_sys(const char* msg);

int main(int argc, char** argv)
{
    if (argc != 2) {
        printf("usage server <port>\n");
        exit(EXIT_FAILURE);
    }
    int listen_sock, client_sock, rv;
    struct sockaddr_in server_sinaddr, client_sinaddr;

    //server side network configuration
    server_sinaddr.sin_family = AF_INET;
    server_sinaddr.sin_port = htons((in_port_t)strtoul(argv[1], NULL, 10));
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

    //getting value from server side
    char buf[BUFFER_SIZE + 1] = { 0 };
    ssize_t response;
    for (;;) {
        response = recv(client_sock, buf, BUFFER_SIZE, 0);
        if (response == -1)
            exit_sys("recv");
        //is client socket closed
        else if (response == 0)
            break;
        //writing null character
        buf[response] = '\0';
        //if recive exit then finish conversation
        if (!strcmp(buf, "exit")) {
            printf("quit!\n");
            break;
        }

        puts(buf);
    }

    close(listen_sock);
    close(client_sock);

    return 0;
}

void exit_sys(const char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}