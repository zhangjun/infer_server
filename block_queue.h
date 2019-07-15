/*******************************************
   Author        : Jun Zhang
   Email         : ewalker.zj@gmail.com
   Last Modified : 2019-01-05 19:12
   Filename      : block_queue.h
   Description   : 
*******************************************/
#pragma once

#include <mutex>
#include <condition_variable>
#include <thread>
#include <deque>

#include <chrono>

#include <boost/noncopyable.hpp>

#include <iostream>

namespace common {

double GetTime();

template<typename T>
class BlockingQueue : boost::noncopyable {
    public:
        BlockingQueue(uint32_t max_elements = static_cast<uint32_t>(-1))
            : max_elements_(max_elements) {
                //assert(max_elements > 0);
        }

        bool push_front(const T& item){
            bool ret = false;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                if(!unlocked_is_full()){
                    queue_.push_front(item);
                    ret = true;
                }
            }
            not_empty_condition_.notify_one();

            return ret;
        }

        bool push_back(const T& item){
            bool ret = false;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                if(!unlocked_is_full()){
                    queue_.push_back(item);
                    ret = true;
                }
            }
            not_empty_condition_.notify_one();

            return ret;
        }

        void pop_front(T& item){
            {
                std::unique_lock<std::mutex> lock(mutex_);
                while(queue_.empty()){
                    std::cout << "waiting" << std::endl;
                    not_empty_condition_.wait(lock);
                }

                item = queue_.front();
                queue_.pop_front();
                std::cout << "pop front " << std::endl;
            }
            not_full_condition_.notify_one();
        }

        void pop_back(T& item){
            {
                std::unique_lock<std::mutex> lock(mutex_);
                while(queue_.empty()){
                    not_empty_condition_.wait(lock);
                }

                item = queue_.back();
                queue_.pop_back();
            }
            not_full_condition_.notify_one();
        }

        bool pop_front_timed(T& item, const uint32_t timeout = 5){
            bool success = false;
            {
                double begin = GetTime();
                std::unique_lock<std::mutex> lock(mutex_);

                // timeout
                //while(queue_.empty()){

                while(queue_.empty() && (uint32_t)(GetTime() - begin) < timeout){
                    std::cout << "waiting" << std::endl;
                    //not_empty_condition_.wait(lock);
                    not_empty_condition_.wait_for(lock, std::chrono::microseconds(100));
                    //usleep(5000 * 1000);
                    //std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    std::this_thread::sleep_for(std::chrono::microseconds(2));
                    
                }

                if(!queue_.empty()){
                    item = queue_.front();
                    queue_.pop_front();
                    success = true;
                    std::cout << "pop front " << std::endl;
                }
            }
            if(success)
                not_full_condition_.notify_one();

            return success;
        }

        bool pop_back_timed(T& item, const uint32_t timeout = 10){
            bool success = false;
            {
                double begin = GetTime();
                std::unique_lock<std::mutex> lock(mutex_);

                // timeout
                //while(queue_.empty()){

                while(queue_.empty() && (uint32_t)(GetTime() - begin) < 10){
                    //not_empty_condition_.wait(lock);
                    not_empty_condition_.wait_for(lock, std::chrono::microseconds(200));
                }

                if(!queue_.empty()){
                    item = queue_.back();
                    queue_.pop_back();
                    success = true;
                }
            }
            if(success)
                not_full_condition_.notify_one();

            return success;
        }

        bool is_full() const {
            std::unique_lock<std::mutex> lock(mutex_);
            return unlocked_is_full();
        }

        uint32_t size() {
            std::unique_lock<std::mutex> lock(mutex_);
            return queue_.size();
        }

    private:
        bool unlocked_is_full() const {
            return queue_.size() > max_elements_;
        }

    private:
        std::mutex mutex_;
        std::condition_variable not_empty_condition_;
        std::condition_variable not_full_condition_;
        uint32_t max_elements_;

        std::deque<T> queue_;
};

// typedef  BlockingQueue<T> QueueType
// typedef typename QueueType::value_type value_type
}
