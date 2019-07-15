/*******************************************
   Author        : Jun Zhang
   Email         : ewalker.zj@gmail.com
   Last Modified : 2019-06-12 21:01
   Filename      : infer_task.h
   Description   : 
*******************************************/
#pragma once

#include "base_task.h"


class InferTask : public BaseTask {
    public:
        InferTask(){}
        void Run();

        bool Read();
        bool Write();
};
