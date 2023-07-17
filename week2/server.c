#include <stdio.h>
#include <stdlib.h>
#include <ev.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>

#define BUFFER_SIZE 1024
#define PORT 1113

void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);

void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
    if (EV_ERROR & revents)
    {
        perror("got invalid event");
        return;
    }

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(watcher->fd, (struct sockaddr *)&client_addr, &client_len);

    if (client_fd < 0)
    {
        perror("accept error");
        return;
    }

    struct ev_io *read_watcher = (struct ev_io *)malloc(sizeof(struct ev_io));
    ev_io_init(read_watcher, read_cb, client_fd, EV_READ);
    ev_io_start(loop, read_watcher);
}

void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
    if (EV_ERROR & revents)
    {
        perror("got invalid event");
        return;
    }

    char buffer[BUFFER_SIZE];
    ssize_t read;

    read = recv(watcher->fd, buffer, sizeof(buffer), 0);

    if (read < 0)
    {
        perror("read error");
        return;
    }

    if (read == 0)
    {
        // Connection closed by client
        ev_io_stop(loop, watcher);
        free(watcher);
        printf("Client disconnected\n");
        return;
    }

    printf("Received message: %.*s", (int)read, buffer);

    // Echo the received message back to the client
    send(watcher->fd, buffer, read, 0);
    memset(buffer, 0, sizeof(buffer));
}
void print_current_time_cb(struct ev_loop *loop, ev_timer *w, int revents) //loop為循環監聽  w最初幾秒開始  revents 幾秒重複一次
{
    time_t now;
    struct tm *tm_now;
    char time_str[20];

    now = time(NULL);
    tm_now = localtime(&now);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_now);

    printf("Current time: %s\n", time_str);
}
int main()
{
    struct ev_loop *loop = ev_default_loop(0);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket creation failed");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind failed");
        return 1;
    }

    if (listen(server_fd, 10) < 0)
    {
        perror("listen failed");
        return 1;
    } 
    struct ev_io accept_watcher;
    struct ev_timer time_watcher;

    ev_io_init(&accept_watcher, accept_cb, server_fd, EV_READ);
    ev_io_start(loop, &accept_watcher);
    
    ev_timer_init(&time_watcher, print_current_time_cb, 5.0, 5.0); //第一個為觸發後幾秒開始 第二參數為每幾秒一次
    ev_timer_start(loop, &time_watcher);


    ev_run(loop, 0);

    return 0;
}