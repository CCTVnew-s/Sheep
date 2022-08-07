#ifndef TOOOOOOOL
#define TOOOOOOOL

#include <chrono>
#include <string>
#include "logging.h"
#include <iostream>

class simpletimer{
public:
    simpletimer(std::string id):timeditem(id){
        start = std::chrono::high_resolution_clock::now();
    };

    ~simpletimer(){
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::nanoseconds elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        LOGAndCOUT(INFO,TIMER, "" << timeditem.c_str() <<  " action has take "<< elapsed.count() << " nano seconds" << std::endl);
    };

    std::chrono::high_resolution_clock::time_point start;
    std::string timeditem;

};





#endif