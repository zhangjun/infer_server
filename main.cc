/*******************************************
   Author        : Jun Zhang
   Email         : ewalker.zj@gmail.com
   Last Modified : 2019-01-06 19:01
   Filename      : main.cc
   Description   : 
*******************************************/
//#include "block_queue.h"
//#include "thread_pool.h"

#include "base_server.h"

#include "echo_task.h"
#include "recog_engine.h"

class Fn {
    public:
        void Handle(std::vector<std::string> &data);
};

void Fn::Handle(std::vector<std::string> &data) {
    int i = 0;
    for(auto &it : data) {
        usleep(10 * 1000);
        it += std::to_string(i ++ );
    }
}

int main(){

    //BaseServer<EchoTask, RecogEngine> server(10, 2019);
    BaseServer<EchoTask, Fn> server(10, 2019);
    server.Init();
    server.Run();
    return 0;
}
