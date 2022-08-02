#include "k.h"
#include "variablestore.h"
#include "signalplatform.h"
#include "signalplatformtest.h"
#include "memorymanager.h"
#include <iostream>
#include <thread>
#include <cstring>

#include <unistd.h>
#include <limits.h>

#include "testlist.h"


// program configure
#define NUMTHREAD 5
#define KTIMEOUT 600000



bool testpart(RUNENV e, I handle, MemoryManager *m ){
    // testthreadpool(1,NULL);
    // ktoounittest(e);
    testcalculator( handle, e,m);
    return false;
}


RUNENV getenv(){
    char hostname[HOST_NAME_MAX];
    char username[LOGIN_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    getlogin_r(username, LOGIN_NAME_MAX);
    std::string host(hostname);
    return host.find("10297") == std::string::npos ? RUNENV::h1 : RUNENV::w1;
};





int main(){
    RUNENV e = getenv();

    std::cout << "running at" << int(e) << std::endl;

   // const auto host_name = boost::asio::ip::host_name();



    I datahandle = khpunc("localhost", e==RUNENV::h1?8939:9002, "",KTIMEOUT, 1);    

    auto memorymgr = buildnmemorymgr(NUMTHREAD + 1,std::vector<CalculationLevel>{CalculationLevel::Top, CalculationLevel::Date, CalculationLevel::Signal},
    
    std::vector<MemoryLifeCyle>{MemoryLifeCyle::Global, MemoryLifeCyle::LoopTask, MemoryLifeCyle::SinglePhase},1024L*1024L*1024L*2L);
    
    ctpl::thread_pool workers(NUMTHREAD);

    LogByThread::initializeservice(NUMTHREAD);


    // testing use all service    
    testpart(e, datahandle,memorymgr.at(0));


    // FULL Memory and Full LOG/THREAD, any valid config should work
    // I datahandle = khpunc("localhost", 8939, "",1000, 1);


    // globalcontext->insert(std::make_pair(RecursiveIterationExecutor::MEMEORYMANAGERS_key, new kfc(kfc{KPtr,{.p = (void *)&memorymgr}})));
    // globalcontext->insert(std::make_pair(RecursiveIterationExecutor::THREADPOOLS_key,  new kfc(kfc{KPtr,{.p = (void *)&workers}})));
    KFCStore *globalcontext = new KFCStore();
    KFC roottask = new kfc(kfc{KS,{.s="TestConfigure"}});
    globalcontext->insert(std::make_pair("root",roottask));
 
    auto configs = buildtestconfig2("firstleveliterator");
 
    MultiLoopPlatform platform(configs.first, configs.second, globalcontext,"firstleveliterator",&memorymgr, &workers,roottask);
    // platform.startCalc();
    return 0;
}