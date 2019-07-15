/*******************************************
   Author        : Jun Zhang
   Email         : ewalker.zj@gmail.com
   Last Modified : 2019-06-12 21:20
   Filename      : base_server_impl.h
   Description   : 
*******************************************/
//#include "base_server.h"
#include "string_util.h"


#include <arpa/inet.h>

// https://github.com/chennj/middleserver/blob/master/middleserver/epoll_server.cpp
template <class T>
bool BaseServer<T>::is_stop_ = false;

template <class T>
bool BaseServer<T>::Run(){
    Loop();
}

template <class T>
bool BaseServer<T>::Init(){
    listen_fd_ = init_listen(listen_port_);
    if(listen_fd_ < 0)
        return false;

    epoll_fd_ = epoll_create1(0);
    if(epoll_fd_ < 0){
        // epoll_create failed
        return false;
    }

    // create thread pool
    for(int i = 0; i < 4; i ++){
        pools_[i] = new common::ThreadPool<T>(i + 1);
    }
}
template <class T>
bool BaseServer<T>::Loop(){
    //for(auto it : pools_){
    //    (it.second) -> start();
    //}

    add_fd(epoll_fd_, listen_fd_, false);

    while(!is_stop_){
        int ret = epoll_wait(epoll_fd_, events_, MAX_EVENT, -1);
        if(ret < 0){
            // epoll_wait error
            continue;
        }

        for(int i = 0; i < ret; i ++){
            int fd = events_[i].data.fd;
            if (fd == listen_fd_) {
                struct sockaddr_in client_addr;
                socklen_t len = sizeof(client_addr);
                int conn_fd = accept(listen_fd_, (struct sockaddr *)&client_addr, &len);
                if(conn_fd < 0)
                    continue;

                add_fd(epoll_fd_, conn_fd, false);
                
                std::cout << "new conn ......." << std::endl;

            } else if (events_[i].events & EPOLLIN) {
                EString buffer;
                unsigned int len, tag;

                char header[8];

                if(!recv_data(header, 8, fd)){

                } else {
                }

                len = string_util::bytes_to_int_little(header);
                tag = string_util::bytes_to_int_little(header + 4);

                std::cout << "len: " << len << ", tag: " << tag << std::endl;

                int data_len = recv_data(buffer.getBuffer(), len, fd);
                if(data_len <= 0){

                } else {
                    // process data
                    buffer.setLength(data_len);
                    buffer.getBuffer()[data_len] = '\0';

                    std::cout << "data len: " << buffer.length() << ", " << buffer.getBuffer() << std::endl;
                    pools_[tag] -> add_task(T(fd, epoll_fd_, buffer.getBuffer(), buffer.length()));
                }


            } else if((events_[i].events & EPOLLERR) ||
                      (events_[i].events & EPOLLHUP) ||
                      (!(events_[i].events & EPOLLIN))) {

                close(events_[i].data.fd);
                events_[i].data.fd = -1;
            }

        }
    }

    close(listen_fd_);

    //for(auto it : pools_){
    //    (it.second) -> stop();
    //}

}


