/*******************************************
   Author        : Jun Zhang
   Email         : ewalker.zj@gmail.com
   Last Modified : 2019-06-15 17:06
   Filename      : block_queue.cc
   Description   : 
*******************************************/
#include "block_queue.h"

namespace common {

double GetTime(){
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1000000.0;
}

/**
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
                    not_empty_condition_.wait(lock);
                }

                item = queue_.front();
                queue_.pop_front();
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

        bool pop_front_timed(T& item, const uint32_t timeout = 10){
            bool success = false;
            {
                double begin = GetTime();
                std::unique_lock<std::mutex> lock(mutex_);

                // timeout
                //while(queue_.empty()){

                while(queue_.empty() && (uint32_t)(GetTime() - begin) < 10){
                    not_empty_condition_.wait(lock);
                }

                if(!queue_.empty()){
                    item = queue_.front();
                    queue_.pop_front();
                    success = true;
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
                    not_empty_condition_.wait(lock);
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

*/

}
