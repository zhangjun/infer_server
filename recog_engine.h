/*******************************************
   Author        : Jun Zhang
   Email         : ewalker.zj@gmail.com
   Last Modified : 2019-06-18 20:56
   Filename      : recog_engine.h
   Description   : 
*******************************************/
#pragma once 

#include "base_engine.h"

class RecogEngine : public BaseEngine {
    public:
        virtual void Run();

    private:
        int device_id;
        int config_name;
};


