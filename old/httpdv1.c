#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8080
#define RECV_BUF_SIZE 2048
#define OPEN_FILE "index.html"

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

    if (listen(sockfd, 8) == -1) {
        perror("listen");
        close(sockfd);
        return -1;
    }

    return sockfd;
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

int send_response(int c_sock, const char *status, const char *body) {
    size_t body_size = strlen(body);
    char length[64];
    snprintf(length, sizeof(length), "Content-Length: %zu\r\n", body_size);
    const char *header =
        "Content-Type: text/html\r\n"
        "Connection: close\r\n"
        "\r\n";

    send_all(c_sock, status, strlen(status));
    send_all(c_sock, length, strlen(length));
    send_all(c_sock, header, strlen(header));
    send_all(c_sock, body, strlen(body));

    return 0;
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

    if (strncmp(recv_buf, "GET / ", 6) == 0) {
        send_response(
            c_sock,
            "HTTP/1.1 200 OK\r\n",
            "<h1>Home</h1>"
        );
        return 0;
    }

    if (strncmp(recv_buf, "GET /hello ", 11) == 0) {
        send_response(
            c_sock,
            "HTTP/1.1 200 OK\r\n",
            "<h1>Hello</h1>"
        );
        return 0;
    }

    if (strncmp(recv_buf, "GET /index.html ", 16) == 0) {
        FILE *fp;

        fp = fopen(OPEN_FILE, "r");
        if (fp == NULL) {
            send_response(c_sock,"HTTP/1.1 404 Not Found\r\n", "<h1>File Not Found</h1>");
            return -1;
        }
        // get file size 
        fseek(fp, 0, SEEK_END);
        int f_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        char content[f_size+2];

        size_t n = fread(content, 1, sizeof(content) - 1, fp);
        content[n] = '\0';
        fclose(fp);

        send_response(
            c_sock,
            "HTTP/1.1 200 OK\r\n",
            content
        );
        return 0;
    }

    send_response(
        c_sock,
        "HTTP/1.1 404 Not Found\r\n",
        "<h1>404 Not Found</h1>"
    );
    return 0;
}

int main() {

    int sockfd = create_server_socket();
    if (sockfd == -1) {
        printf("creating socket failed\n");
        return -1;
    }

    int c_sock;
    while (1) {
        c_sock = accept(sockfd, NULL, NULL);
        if (c_sock == -1) {
            perror("accept");
            continue;
        }

        handle_client(c_sock);
        close(c_sock);
    }
}