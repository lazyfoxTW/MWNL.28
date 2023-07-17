#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ev.h>

#define BUFFER_SIZE 1024

struct ev_loop *loop;

void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
    if (EV_ERROR & revents)
    {
        perror("Invalid event");
        return;
    }

    char buffer[BUFFER_SIZE];
    ssize_t read_size = recv(watcher->fd, buffer, BUFFER_SIZE, 0);
    if (read_size <= 0)
    {
        if (read_size == 0)
        {
            printf("Server closed the connection\n");
        }
        else
        {
            perror("recv error");
        }
        ev_io_stop(loop, watcher);
        free(watcher);
        return;
    }

    printf("Received message from server: %s\n", buffer);
    memset(buffer, 0, BUFFER_SIZE);
}

int main()
{
    loop = ev_default_loop(0);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket creation failed");
        return EXIT_FAILURE;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(1113);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("connect failed");
        close(sockfd);
        return EXIT_FAILURE;
    }

    printf("Connected to server.\n");

    struct ev_io *w_recv = (struct ev_io *)malloc(sizeof(struct ev_io));
    ev_io_init(w_recv, read_cb, sockfd, EV_READ);
    ev_io_start(loop, w_recv);

    char message[BUFFER_SIZE];
    while (1)
    {
        printf("Enter message: ");
        fgets(message, BUFFER_SIZE, stdin);

        if (strcmp(message, "exit\n") == 0)
            break;

        send(sockfd, message, strlen(message), 0);
        memset(message, 0, sizeof(message));
    }

    close(sockfd);
    free(w_recv);
    ev_loop_destroy(loop);

    return 0;
}