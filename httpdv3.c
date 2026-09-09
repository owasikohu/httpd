#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8080
#define BACKLOG 8
#define RECV_BUF_SIZE 2048

#define PUBLIC_DIR "public"


int create_server_socket() {
    int sockfd;
    struct sockaddr_in addr = {0};

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_ADDR, &addr.sin_addr) != 1) {
        perror("inet_pton");
        return -1;
    }

    if (bind(sockfd, (const struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(sockfd);
        return -1;
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int handle_client(int c_sock) {
    int recv_size;
    char recv_buf[RECV_BUF_SIZE];

    recv_size = recv(c_sock, recv_buf, RECV_BUF_SIZE - 1, 0);

    if (recv_size == -1) {
        perror("recv");
        return -1;
    }

    if (recv_size == 0) {
        return 0;
    }

    recv_buf[recv_size] = '\0';


    char method[16];
    char path[256];
    char version[16];

    sscanf(recv_buf, "%15s %255s %15s", method, path, version);  

  
}

int send_all(int sock, const char *buf, size_t len) {
    size_t sent = 0;

    while (sent < len) {
        ssize_t n = send(sock, buf + sent, len - sent, 0);

        if (n == 0) {
            return -1;
        }
        if (n == -1) {
            perror("send");
            return -1;
        }
    

        sent += n;
    }

    return 0;
}


int main() {

}