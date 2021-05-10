#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define SERVER_PORT 5050
#define BUFFER_SIZE 1024

typedef struct tagCLIENT_INFO {
    int sock;
    struct sockaddr_in sinaddr;
}CLIENT_INFO;



void exit_sys(const char* msg);
char* rev_str(char* str);
void* client_proc(void* param);

int main(int argc, char** argv)
{
    int listen_sock, client_sock, rv;
    struct sockaddr_in server_sinaddr, client_sinaddr;
    pthread_t tid;
    int tresult;
    CLIENT_INFO* cli; 

    if (argc != 2) {
        printf("usage server <port>\n");
        exit(EXIT_FAILURE);
    }

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

    printf("Server started..\n");

    for (;;) {

        socklen_t client_sinaddr_len = sizeof(client_sinaddr);

        client_sock = accept(listen_sock, (struct sockaddr*)&client_sinaddr, &client_sinaddr_len);
        if (client_sock == -1)
            exit_sys("accept");

        cli = (CLIENT_INFO*) malloc(sizeof(CLIENT_INFO));
        cli->sinaddr = client_sinaddr;
        cli->sock = client_sock;
        
        tresult = pthread_create(&tid, NULL, client_proc, (void*)cli);
        if (tresult != 0)
            exit_sys("pthread_create");

        tresult = pthread_detach(tid);
        if (tresult != 0)
            exit_sys("pthread_detach");
    }

    close(listen_sock);
    return 0;
}

void exit_sys(const char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

char* revStr(char* str)
{
    char temp;
    int i, k;

    for (i = 0, k = strlen(str) - 1; i < k; i++, k--) {
        temp = str[i];
        str[i] = str[k];
        str[k] = temp;
    }

    return str;
}

void* client_proc(void* param)
{
    ssize_t result;
    CLIENT_INFO* cli = (CLIENT_INFO*) param;
    char buf[256];
    for (;;) {
        result = recv(cli->sock, buf, BUFFER_SIZE, 0);
        if (result == 0)
            exit_sys("recv");
      
        buf[result] = '\0';
        if (!strcmp(buf, "quit"))
            break;
      
        printf("%s:%u %s\n",inet_ntoa(cli->sinaddr.sin_addr),  htons(cli->sinaddr.sin_port), buf);
        
        revStr(buf);
        result = send(cli->sock, buf, strlen(buf), 0);
        if (result == -1)
            exit_sys("send");
    }
    free(cli);
    shutdown(cli->sock, SHUT_RDWR);
    close(cli->sock);
    return NULL;
}
