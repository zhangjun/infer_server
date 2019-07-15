/*******************************************
   Author        : Jun Zhang
   Email         : ewalker.zj@gmail.com
   Last Modified : 2019-06-15 00:36
   Filename      : string_util.h
   Description   : 
*******************************************/
#pragma once

namespace string_util {

    // big endian
    unsigned int bytes_to_int_big(const char *bytes){
        unsigned int num = 0;
        num |= (bytes[3] & 0xff);
        num |= ((bytes[2] << 8) & 0xff00);
        num |= ((bytes[1] << 16) & 0xff0000);
        num |= ((bytes[0] << 24) & 0xff000000);
        return num;
    }

    void int_to_bytes_big(int val, char* data) {
        data[3] = val & 0xff;
        data[2] = (val >> 8) & 0xff;
        data[1] = (val >> 16) & 0xff;
        data[0] = (val >> 24) & 0xff;
    }

    // little endian
    unsigned int bytes_to_int_little(const char *bytes){
        unsigned int num = 0;
        num |= (bytes[0] & 0xff);
        num |= ((bytes[1] << 8) & 0xff00);
        num |= ((bytes[2] << 16) & 0xff0000);
        num |= ((bytes[3] << 24) & 0xff000000);
        return num;
    }

    void int_to_bytes_little(int val, char* data) {
        data[0] = val & 0xff;
        data[1] = (val >> 8) & 0xff;
        data[2] = (val >> 16) & 0xff;
        data[3] = (val >> 24) & 0xff;
    }

}
