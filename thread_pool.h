/*******************************************
   Author        : Jun Zhang
   Email         : ewalker.zj@gmail.com
   Last Modified : 2019-01-06 20:32
   Filename      : thread_pool.h
   Description   : 
*******************************************/
#pragma once

#include <thread>
#include <vector>
#include <boost/noncopyable.hpp>
#include <assert.h>
//#include "block_queue.h"
#include "wait_list.hpp"

#include "util.h"

namespace common {
struct ThreadTask {
    std::function<void ()> task_func;
    void *args;
};

template <class T, class R> 
class ThreadPool : boost::noncopyable {
public:
    ThreadPool(const uint32_t num = 4) : is_stop_(true), thread_num_(num){
        start();
    }

    ~ThreadPool(){
        is_stop_ = true;
        stop_all();
        task_list_.flush();
    }

    void add_task(T* task){
        if(!is_stop_)
            //task_queue_.push_back(task);
            task_list_.put(task);
    }

    uint32_t wait_nums(){
        //return task_queue_.size();
        return task_list_.len();
    }
    
private:
    void start(){
        assert(thread_num_ > 0);

        for(uint32_t i = 0; i < thread_num_; i ++){
            R* rs = new R();
            resource_worker_.push_back(rs);
        }

        for(uint32_t i = 0; i < thread_num_; i ++){
            std::shared_ptr<std::thread> thread(new std::thread(std::bind(&ThreadPool::run, this, resource_worker_[i])));
            thread_worker_.push_back(thread);
        }
        is_stop_ = false;
    }

    void run(R* r){
        while(!is_stop_){
            T task;
            double begin = GetTime();
            //std::unordered_map<int, T> batch_items;
            std::vector<T*> batch_items;

            int max_width = 3600, width_sum = 0;
            while((uint32_t)(GetTime() - begin) < 50){
                T* task = (T*)task_list_.get_from_head();
                if(task == NULL) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    continue;
                }
                //std::cout << "queue pop , " << task_queue_.size() << std::endl;
                //if(!task_queue_.pop_front_timed(task, 5)){
                //    std::this_thread::sleep_for(std::chrono::microseconds(20));
                //    continue;
                //}
                ////task_queue_.pop_front(task);
                ////batch_items[task.listen_fd()] = task;
                std::cout << "width: " << task -> width() << std::endl;
                if(width_sum + task -> width() > max_width) {
                    task_list_.put_front(task);
                    break;
                }
                batch_items.push_back(task);
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }

            std::cout << "runing, wait_num: " << wait_nums() << ", fetch_num: " << batch_items.size() << std::endl;
            std::vector<std::string> data;
            for(auto it : batch_items){
                //it.second.Run();
                data.push_back(it -> item_data());
                //it -> Run();
            }
            //task.Run();
            r -> Handle(data);

            int i = 0;
            for(auto it : batch_items) {
                it -> send_data(data[i ++]);
            }
            //if(task.Run != nullptr){
            //    task.Run();
            //}
        }
    }

    void stop_all(){
        for(uint32_t i = 0; i < thread_worker_.size(); i ++){
            thread_worker_[i] -> join();
        }
    }

private:
    bool is_stop_;
    uint32_t thread_num_;
    //BlockingQueue<T> task_queue_;
    wait_list_t task_list_;
    std::vector<std::shared_ptr<std::thread>> thread_worker_;

    std::vector<R*> resource_worker_;
};
}
