// Thread split log

#ifndef LOGGING
#define LOGGING

#include <map>
#include <thread>
#include <iostream>
#include <fstream>


class LogByThread{
public:
    static std::map<std::thread::id, int> lookuptable;
    static std::map<int, std::fstream * > logs;

    static const int DEFAULTSTREAM;


    static bool initializeservice(int n){

        // simplest logs go to ID labeled files
        for (int i=0;i<n;i++)
           {
               logs.insert(std::make_pair(i,  new std::fstream(std::string("logs/thread") + std::to_string(i) + std::string(".log"),std::ios::out | std::ios::trunc)));
               (*logs.at(i)) << "longging " << i << " started \n"; 
           }
        logs.insert(std::make_pair(LogByThread::DEFAULTSTREAM, new std::fstream(std::string("logs/thread") + std::to_string(LogByThread::DEFAULTSTREAM) + std::string(".log"),std::ios::out | std::ios::trunc)));
        return true;
    };

    static bool endservice(){
        for (auto s : logs){
            (*s.second).flush();
            (*s.second).close();
        }
 
    };

    static bool bundleidwithID(std::thread::id cid, int ID){
        if (lookuptable.find(cid) == lookuptable.end()){
            lookuptable.insert(std::make_pair(cid, ID));
            return true;
        }
        else
            return false;
    };

    static std::fstream& getcurrentlog(){
        std::thread::id currentid = std::this_thread::get_id();
        if (lookuptable.find(currentid) == lookuptable.end()){
             return *logs.at(DEFAULTSTREAM);
        }
        else
            return *logs.at(lookuptable.at(currentid));
    };

};


#define LOG(level, LOGID, info)  (LogByThread::getcurrentlog() << #level << "_" << #LOGID << " :" << info) 
#define LOGAndCOUT(level, LOGID, info)  LogByThread::getcurrentlog() << #level << "_" << #LOGID << " :" << info; std::cout<< #level << "_" << #LOGID << " :" << info


#endif