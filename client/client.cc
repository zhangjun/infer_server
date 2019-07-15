#include <stdio.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <sys/epoll.h>  
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <cstdlib>

#include <string>

#include "echo_service.pb.h"
  
#define MAX_EVENTS 10  

static int widths[5] = {150, 200, 500, 1200, 2400};
 
struct Header {
    unsigned int len;
    unsigned int flag;
};
 
int main()  
{  
    srand((unsigned)time(NULL));
    // socket
    struct sockaddr_in servaddr;  
    short port = 2019;  
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);  
    servaddr.sin_family = AF_INET;  
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  
    servaddr.sin_port = htons(port);
 
    if (connect(sockfd, (sockaddr *) &servaddr, sizeof(sockaddr_in)) < 0) {
        perror("connect fail");
        exit(EXIT_FAILURE);
    }
 
    const char* buf = "daiyudong";
    Header head;
    head.len = strlen(buf);
    head.flag = 2;

    char buffer[10240] = {0};
    char recv_buffer[10240] = {0};

    echo::Request request;
    
    echo::Request response;
 
    int idx = 0;
    for (;;) {
    //for (int i = 0; i < 1; i++) {
        std::string data = "client ";
        data = data + std::to_string(idx++);
        request.set_req_id(idx);
        request.set_data(data);
        int idx = rand() % 5;
        request.set_width_val(widths[idx]);
        std::string content;
        request.SerializeToString(&content);

        head.len = content.size();

        memcpy(buffer, (char*)&head, sizeof(head));
        memcpy(buffer + sizeof(head), content.c_str(), content.size());
        int len = (int)write(sockfd, buffer, content.size() + sizeof(head));
        if (len > 0) {
            printf("write len=%d, request, reqid=%d\n", len, request.req_id());
        }

        int recv_len = read(sockfd, recv_buffer, 8);
        printf("recv len=%d\n", recv_len);

        int l = 0;
        l |= (recv_buffer[0] & 0xff);
        l |= ((recv_buffer[1] << 8) & 0xff00);
        l |= ((recv_buffer[2] << 16) & 0xff0000);
        l |= ((recv_buffer[3] << 24) & 0xff000000);

        if (l <= 0)
            continue;
        recv_len = read(sockfd, recv_buffer, l);
        printf("recv len=%d, l: %d\n", recv_len, l);
        // parse response
        {
            if(response.ParseFromArray(recv_buffer, recv_len)) {
                printf("response, req_id: %d, width: %d, data: %s\n", response.req_id(), response.width_val(), response.data().c_str());
            }
        }
        //printf("recv data: %s\n", recv_buffer);
        usleep(50 * 1000);
    }
}  
