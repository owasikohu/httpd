#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8080
#define BUF_SIZE 2048

int main() {
    int sockfd;
    int csock;
    struct sockaddr_in addr = {0};

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Error: socket\n");
        return 1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons((unsigned short)SERVER_PORT);
    addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    
    if (bind(sockfd, (const struct sockaddr *)&addr, sizeof(addr)) == -1) {
        printf("Error: bind\n");
        close(sockfd);
        return 1;
    }

    if (listen(sockfd, 8) == -1) {
        printf("Error: listen\n");
        close(sockfd);
        return 1;
    }

    while (1) {
        csock = accept(sockfd, NULL, NULL);
        if (csock == -1) {
            printf("Error: accept\n");
            close(sockfd);
            return 1;
        }
        printf("Connected\n");

        int send_size;
        int recv_size;
        char recv_buf[BUF_SIZE];
        char send_buf[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n<h1>Hello from C</h1>";

        recv_size = recv(csock, recv_buf, BUF_SIZE - 1, 0);

        if (recv_size == -1) {
            printf("Error: recv\n");
        } else if (recv_size == 0) {
            printf("Connection ended\n");
        } else {
            recv_buf[recv_size] = '\0';
            printf("%s", recv_buf);
        }

        send_size = send(csock, send_buf, sizeof(send_buf) - 1, 0);
        close(csock);
    }
}