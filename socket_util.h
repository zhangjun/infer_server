/*******************************************
   Author        : Jun Zhang
   Email         : ewalker.zj@gmail.com
   Last Modified : 2019-06-06 12:32
   Filename      : socket_util.h
   Description   : 
*******************************************/
#pragma once

#include <stdlib.h>
#include <string.h>  // memset
#include <fcntl.h>
#include <sys/socket.h>

#include <netinet/in.h>  // htons

#include <unistd.h>  // close, usleep

#include <sys/epoll.h>
#include <sys/wait.h>

#include <errno.h>
#include <stdio.h>


#include <string>

#include "fixed_string.h"

enum SockStatus {
    NOT_USED = 0,
    READY,
    READ_BUSY,
    BUSY,
    WRITE_BUSY
};

struct SockItem {
    int sock;
    unsigned int fd;
    time_t time;
    int sock_status;
    EString in;
    EString out;
    std::string data;
    //unsigned int* recv_buf;
};

int init_listen(int port);
int set_nonblock(int fd);

void on_sigint(int signal);

void add_fd_val(int epoll_fd, int socket_fd, int fd_val, bool oneshot);

void add_fd(int epoll_fd, int socket_fd, bool oneshot);

void reset_oneshot(int epoll_fd, int socket_fd);


// handle data
int recv_data(char* buffer, int size, int fd);

int send_data(const char* buffer, int size, int fd);


