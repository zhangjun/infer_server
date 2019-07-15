/*******************************************
   Author        : Jun Zhang
   Email         : ewalker.zj@gmail.com
   Last Modified : 2019-06-12 20:39
   Filename      : base_server.h
   Description   : 
*******************************************/
#pragma once

#include <iostream>
#include <unordered_map>
#include <climits>

#include "thread_pool.h"
#include "socket_util.h"
#include "fixed_string.h"
#include "simple_queue.h"

#include "constants.h"

#define MAX_EVENT 2048
#define MAX_SOCK 2048
#define MAX_QUEUELEN 1024

// https://www.cnblogs.com/moonlove/archive/2012/03/17/2509150.html

template <class T, class R>
class BaseServer {
    public:
        virtual bool Run();
        virtual bool Init();
        virtual bool Loop();

    public:
        BaseServer() : thread_num_(10), listen_port_(2019), read_status_(0), sock_items_(NULL), evfd_queue_(NULL){}
        BaseServer(int num_thread, int port) : thread_num_(num_thread), listen_port_(port), read_status_(0), sock_items_(NULL), evfd_queue_(NULL) {}

        ~BaseServer() {
            for(auto it : pools_)
                delete it.second;

            if (sock_items_ != NULL){
                delete [] sock_items_;
                sock_items_ == NULL;
            }

            if (evfd_queue_ != NULL){
                delete evfd_queue_;
            }
        }

    protected:
        int check_timeout();
        int get_offset();
        int insert_item(int sock);
        int reset_item(int offset, bool keepalive);

    private:
        int read_status_;
        static bool is_stop_;
        int listen_fd_;
        int listen_port_;
        int thread_num_;
        
        int epoll_fd_;
        epoll_event events_[MAX_EVENT];

        SockItem *sock_items_;
        //int* evfd_offsets_;
        SimpleQueue *evfd_queue_;

        int sock_len_, sock_num_;
        int least_last_update;
        int min_timeout;
        int conn_timeout;
        int read_timeout;
        int write_timeout;

        struct sockaddr_in addr_;

        std::unordered_map<int, common::ThreadPool<T, R>* > pools_;
};

#include "base_server_impl.h"
