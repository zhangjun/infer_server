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

// https://stackoverflow.com/questions/10632251/undefined-reference-to-template-function

// https://github.com/chennj/middleserver/blob/master/middleserver/epoll_server.cpp

template <class T, class R>
bool BaseServer<T, R>::is_stop_ = false;

template <class T, class R>
bool BaseServer<T, R>::Run(){
    Loop();
}

template <class T, class R>
bool BaseServer<T, R>::Init(){
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
        pools_[i] = new common::ThreadPool<T, R>(i + 1);
    }

    sock_len_ = 0, sock_num_ = MAX_SOCK;
    sock_items_ = new SockItem[MAX_SOCK];
    evfd_queue_ = new SimpleQueue(MAX_QUEUELEN);

    conn_timeout = CONN_TIMEO;
    read_timeout = READ_TIMEO;
    write_timeout = WRITE_TIMEO;

    least_last_update = INT_MAX;

    return true;
}

template <class T, class R>
bool BaseServer<T, R>::Loop(){
    //for(auto it : pools_){
    //    (it.second) -> start();
    //}

    //add_fd(epoll_fd_, listen_fd_, false);
    add_fd_val(epoll_fd_, listen_fd_, -1, false);

    while(!is_stop_){
        check_timeout();
        int ret = epoll_wait(epoll_fd_, events_, MAX_EVENT, -1);
        if(ret < 0){
            // epoll_wait error
            continue;
        }

        int offset = -1;
        for(int i = 0; i < ret; i ++){
            int fd = events_[i].data.fd;
            std::cout << "fd: ~~ " << fd << std::endl;

            if (fd == -1 && listen_fd_ > 0) {
                struct sockaddr_in client_addr;
                socklen_t len = sizeof(client_addr);
                int conn_fd = accept(listen_fd_, (struct sockaddr *)&client_addr, &len);
                if(conn_fd < 0)
                    continue;

                if ((offset = insert_item(conn_fd)) < 0) {
                    while(close(conn_fd) < 0 && errno == EINTR);
                    std::cout << "insert item failed." << std::endl;
                    continue;
                }

                add_fd_val(epoll_fd_, conn_fd, offset, false);
                //add_fd(epoll_fd_, conn_fd, false);
                
                std::cout << "new conn ......." << std::endl;

            } else if (events_[i].events & EPOLLIN) {
                EString buffer;
                unsigned int len, tag;

                char header[8];
                
                if(sock_items_[fd].sock_status != READ_BUSY) {
                    sock_items_[fd].sock_status = READ_BUSY;
                    sock_items_[fd].time = time(NULL);
                }

                int sock = sock_items_[fd].sock;
                if(recv_data(header, 8, sock) <= 0){
                    reset_item(fd, false);
                } else {
                }

                len = string_util::bytes_to_int_little(header);
                tag = string_util::bytes_to_int_little(header + 4);

                std::cout << "len: " << len << ", tag: " << tag << ", sock: " << sock << std::endl;

                buffer.reserve(len);
                int data_len = recv_data(sock_items_[fd].in.getBuffer(), len, sock);
                if(data_len <= 0){
                    reset_item(fd, false);

                } else {
                    // process data
                    sock_items_[fd].in.setLength(data_len);
                    sock_items_[fd].in.getBuffer()[data_len] = '\0';

                    std::cout << "data len: " << sock_items_[fd].in.length() << ", " << sock_items_[fd].in.getBuffer() << std::endl;
                    //pools_[tag] -> add_task(T(sock, epoll_fd_, sock_items_[fd].in.getBuffer(), sock_items_[fd].in.length()));
                    //pools_[tag] -> add_task(T(sock, epoll_fd_, sock_items_[fd].in.getBuffer(), sock_items_[fd].in.length()));
                    T* task = new T(epoll_fd_, fd, &sock_items_[fd]);
                    task -> ParseRequest();
                    pools_[tag] -> add_task(task);
                }


            } else if(events_[i].events & EPOLLOUT) {


                if(sock_items_[fd].sock_status != WRITE_BUSY) {
                    sock_items_[fd].sock_status = WRITE_BUSY;
                    sock_items_[fd].time = time(NULL);
                }

                char header[8 + 1] = {0};

                int len = sock_items_[fd].data.size(), tag = 4, write_len = 0;
                
                string_util::int_to_bytes_little(len, header);
                string_util::int_to_bytes_little(tag, header + 4);
                
                send_data(header, 8, sock_items_[fd].sock);

                std::cout << "len: " << len << ", tag: " << tag << ", sock: " << sock_items_[fd].sock << std::endl;
                if(len > 0)
                write_len = send_data(sock_items_[fd].data.c_str(), sock_items_[fd].data.size(), sock_items_[fd].sock);
                //send_data(sock_items_[fd].out.getBuffer(), sock_items_[fd].out.length(), sock_items_[fd].sock);
                
                std::cout << "epollout: " << sock_items_[fd].data << ", write_len: " << write_len << std::endl;

                struct epoll_event ev;
                ev.data.fd = fd;
                ev.events = EPOLLIN | EPOLLET;
                epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, sock_items_[fd].sock, &ev);
                    
            } else if((events_[i].events & EPOLLERR) ||
                      (events_[i].events & EPOLLHUP) ||
                      (!(events_[i].events & EPOLLIN))) {

                //close(events_[i].data.fd);
                reset_item(fd, false);
            }

        }
    }

    close(listen_fd_);

    //for(auto it : pools_){
    //    (it.second) -> stop();
    //}

}


template<class T, class R>
int BaseServer<T, R>::check_timeout(){

    int current_time = time(NULL);
    if (least_last_update != INT_MAX && current_time - least_last_update <= min_timeout) {
        return 0;
    }

    if (sock_items_ == NULL || sock_len_ <= 0 ) {
        return -1;
    }

    int read_num = 0, write_num = 0, busy_num = 0, ready_num = 0, last_active_offset = -1;
    for(int i = 0; i < sock_len_; i ++){
        switch(sock_items_[i].sock_status){
            case READY:
                if(current_time >= sock_items_[i].time + conn_timeout){
                    reset_item(i, false);
                    continue;
                }

                ready_num ++;
                if (least_last_update > sock_items_[i].time) {
                    least_last_update = sock_items_[i].time;
                }
                last_active_offset = i;
                break;
            case READ_BUSY:
                if(current_time >= sock_items_[i].time + read_timeout){
                    reset_item(i, false);
                    continue;
                }

                read_num ++;
                if (least_last_update > sock_items_[i].time) {
                    least_last_update = sock_items_[i].time;
                }
                last_active_offset = i;
                break;
            case WRITE_BUSY:
                if(current_time >= sock_items_[i].time + write_timeout){
                    reset_item(i, false);
                    continue;
                }

                write_num ++;
                if (least_last_update > sock_items_[i].time) {
                    least_last_update = sock_items_[i].time;
                }
                last_active_offset = i;
                break;
            case BUSY:
                busy_num ++;
                sock_items_[i].time = current_time;
                last_active_offset = i;
                break;
        }
    }

    sock_len_ = last_active_offset + 1;

    return 0;
}

template<class T, class R>
int BaseServer<T, R>::get_offset(){
    if(sock_items_ == NULL)
        return -1;

    //std::cout << "sock_len_: " << sock_len_ << std::endl;
    for(int i = 0; i < sock_len_; i ++){
        if(sock_items_[i].sock_status == NOT_USED)
            return i;
    }

    if(sock_len_ >= sock_num_) {
        return -1;
    }

    return sock_len_ ++ ;
}

template<class T, class R>
int BaseServer<T, R>::insert_item(int sock){
    if(sock < 0) {
        return -1;
    }

    int offset = get_offset();
    std::cout << "offset: " << offset << std::endl;
    if( -1 == offset) {
        return -1;
    }

    sock_items_[offset].sock_status = READY;
    sock_items_[offset].time = time(NULL);
    sock_items_[offset].sock = sock;

    return offset;
}

template<class T, class R>
int BaseServer<T, R>::reset_item(int offset, bool keepalive){
    
    if(sock_items_ == NULL || offset < 0 || offset >= sock_len_){
        return -1;
    }
    
    if(sock_items_[offset].sock_status == NOT_USED){
        return 0;
    }

    int sock = sock_items_[offset].sock;
    if(sock < 0) {
        return -1;
    }

    int ret = 0;
    if(!keepalive){

        while((ret = close(sock)) < 0 && errno == EINTR);

        if ( ret < 0 ) {
            return -1;
        }

        sock_items_[offset].sock_status = NOT_USED;
        sock_items_[offset].sock = -1;

    } else {

        sock_items_[offset].sock_status = READY;
        sock_items_[offset].time = time(NULL);

        //epoll_ctl();

    }

    return 0;
}

