/*******************************************
   Author        : Jun Zhang
   Email         : ewalker.zj@gmail.com
   Last Modified : 2019-06-27 19:14
   Filename      : simple_queue.h
   Description   : 
*******************************************/
#pragma once 

#include <cstdio>

class SimpleQueue {
    public:
        SimpleQueue(int size) : get_(0), put_(0), queue_size_(size) {
        }

        int Init() {
            if (queue_size_ <= 0)
                return -1;

            data_ = (int*)malloc(queue_size_ * sizeof(int));
            if (data_ == NULL) {
                return -1;
            }
            return 0;
        }


        size_t Size() const {
            return queue_size_;
        }

        size_t Len() const {
            return (put_ - get_ + queue_size_) % queue_size_;
        }

        bool Full() {
            int next_put = (put_ + 1) % queue_size_;
            return (next_put == get_);
        }

        bool Empty(){
            return (get_ == put_);
        }

        bool Push(int val) {
            if(data_ == NULL || Full() )
                return false;

            data_[put_] = val;
            put_ = (put_ + 1 + queue_size_ ) % queue_size_;
            return true;
        }

        bool Pop(int *val) {
            if(data_ == NULL || Empty() )
                return false;

            *val = data_[get_];
            get_ = (get_ + 1 + queue_size_ ) % queue_size_;
            return true;
        }

        ~SimpleQueue() {
            if(data_ != NULL){
                delete data_;
                data_ = NULL;
            }
        }

    private:
        int get_, put_;
        unsigned int queue_size_;
        int *data_;
};
