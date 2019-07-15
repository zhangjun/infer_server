/*******************************************
   Author        : Jun Zhang
   Email         : ewalker.zj@gmail.com
   Last Modified : 2019-06-06 12:45
   Filename      : handler.cc
   Description   : 
*******************************************/
#include "handler.h"


void *check_timeout(void*){
    for(auto it = sock_items.begin(); it != sock_items.end(); it ++){
        if(time((time_t)0) - (it -> second).time > 120) {
            free((it -> second).recv_buf);
            sock_items.erase(it);
            close(it -> first);
    }
}
