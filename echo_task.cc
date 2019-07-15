/*******************************************
   Author        : Jun Zhang
   Email         : ewalker.zj@gmail.com
   Last Modified : 2019-06-15 14:54
   Filename      : echo_task.cc
   Description   : 
*******************************************/
#include "echo_task.h"

void EchoTask::Run(){
    // handle
    //printf("recv msg: %s\n", input.getBuffer());
    printf("recv msg: %s\n", input.c_str());
}

bool EchoTask::Read(){
}

bool EchoTask::Write(){
}

bool EchoTask::ParseRequest() {
    if(!request_.ParseFromString(input)) {
        return -1;
    }

    width_val = request_.width_val(); 
    req_id = request_.req_id(); 
    body_item = request_.data(); 
}

bool EchoTask::MakeResponse(const std::string &content) {
    response_.set_req_id(req_id);
    response_.set_width_val(width_val);
    response_.set_data(content);
}

bool EchoTask::send_data(const std::string& content) {
    //(sock_item_ -> out).strscat(content.c_str(), content.size());
    printf("out size: %d\n", (sock_item_ -> out).size());
    //(sock_item_ -> out).memcpy(content.c_str(), content.size());
    //printf("content: %s\n", content.c_str());
    MakeResponse(content);
    std::string response_data;
    if(!response_.SerializeToString(&response_data)) {
        
    }
    (sock_item_ -> data).clear();
    (sock_item_ -> data).append(response_data);
    
    struct epoll_event ev;
    ev.data.fd = epoll_offset_;
    ev.events = EPOLLOUT | EPOLLET;
    epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, sock_item_ -> sock, &ev);
}
