/*******************************************
   Author        : Jun Zhang
   Email         : ewalker.zj@gmail.com
   Last Modified : 2019-06-15 14:50
   Filename      : echo_task.h
   Description   : 
*******************************************/
#pragma once

#include <cstdio>
#include <string>

#include "base_task.h"
#include "fixed_string.h"
#include "socket_util.h"
#include "echo_service.pb.h"

class EchoTask : public BaseTask {
    public:
        EchoTask(){}
        //EchoTask(EchoTask &&) = default;
        //EchoTask(const EchoTask &) {
        //    
        //}
        //EchoTask& operator=(const EchoTask&) = default;

        EchoTask(int fd, int efd, char* buffer, unsigned size) :
            sock_fd_(fd), epoll_fd_(efd), input(buffer, size), input_size_(size) {}

        EchoTask(int efd, int eoffset, SockItem* sock_item) :
            epoll_fd_(efd), epoll_offset_(eoffset), sock_item_(sock_item){
                sock_fd_ = sock_item -> sock;
                input = std::string((sock_item_ -> in).getBuffer(), (sock_item_ -> in).length());
                input_size_ = (sock_item_ -> in).length();
            }

        void Run();

        void BatchRun(void*, const void*);

        bool Read();
        bool Write();

        bool ParseRequest();
        bool MakeResponse(const std::string& content);
        bool send_data(const std::string& content);

        int listen_fd() { return sock_fd_;}
        const std::string& input_data() const {
            return input;
        }

        const std::string& item_data() const {
            return body_item;
        }

        int width() const {
            return width_val;
        }
    private:
        //EString input;
        //EString output;

        std::string input;
        std::string output;

        SockItem* sock_item_;

        int sock_fd_;
        int epoll_fd_;
        int epoll_offset_;
        unsigned int input_size_;

        echo::Request request_;
        echo::Response response_;
        int req_id;
        int width_val;
        std::string body_item;
};
