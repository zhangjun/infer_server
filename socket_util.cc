/*******************************************
   Author        : Jun Zhang
   Email         : ewalker.zj@gmail.com
   Last Modified : 2019-06-06 12:36
   Filename      : socket_util.cc
   Description   : 
*******************************************/
#include "socket_util.h"

int init_listen(int port){
    int socket_op = 1;
    struct sockaddr_in addr;

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &socket_op, sizeof(socket_op));

    memset(&addr, 0, sizeof(addr));
    //bzero(&addr, sizeof(addr));
    
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    int ret = bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret < 0){
        //bind fail
        return ret;
    }

    ret = listen(listen_fd, 1024);
    if(ret < 0){
        //listen fail
        return ret;
    }

    set_nonblock(listen_fd);

    return listen_fd;
}

int set_nonblock(int fd){
    int flag;
    flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
    return flag;
}


void on_sigint(int signal){
    exit(0);
}

void add_fd_val(int epoll_fd, int socket_fd, int fd_val, bool oneshot){
    epoll_event event;
    event.data.fd = fd_val;
    event.events = EPOLLIN | EPOLLET;
    if(oneshot) {
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &event);
    set_nonblock(socket_fd);
}


void add_fd(int epoll_fd, int socket_fd, bool oneshot){
    epoll_event event;
    event.data.fd = socket_fd;
    event.events = EPOLLIN | EPOLLET;
    if(oneshot) {
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &event);
    set_nonblock(socket_fd);
}

void reset_oneshot(int epoll_fd, int socket_fd) {
    epoll_event event;
    event.data.fd = socket_fd;
    event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, socket_fd, &event);
}


int recv_data(char* buffer, int size, int fd) {
    int read_len = 0, ret = 0;
    while(read_len < size) {
        ret = recv(fd, buffer + read_len, size - read_len, 0);
        if(ret > 0) {
            read_len += ret;
            continue;
        }

        if(ret == 0){
            printf("read len: %d, errno: %d\n", ret, errno);
            return 0;
        } else {
            if(errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("continue...\n");
                continue;
            }
            printf("continue read, errno: %d\n", errno);
            return 0;
        }

    }
    return read_len;
}

int send_data(const char* buffer, int size, int fd) {
    int send_len = 0, ret = 0;
    if(buffer == NULL) {
        return -1;
    }

    while(send_len < size) {
        ret = send(fd, buffer + send_len, size - send_len, MSG_NOSIGNAL);
        if(ret == -1) {
            if(errno == EAGAIN) {
                usleep(1000);
                continue;
            }

            return -1;
        }

        send_len += ret;
    }

    return send_len;
}


