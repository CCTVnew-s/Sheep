#include "variablestore.h"
#include "signalplatform.h"
#include "signalplatformtest.h"
#include <iostream>
#include <thread>
#include <cstring>


#define NUMTHREAD 5









int main(){

    KFCStore *globalcontext = new KFCStore();
    auto memorymgr = MemoryManager::buildnmemorymgr(NUMTHREAD + 1, 1024*1024*1024*2);
    ctpl::thread_pool workers(NUMTHREAD);
    I datahandle = khpunc("localhost", 8939, "",1000, 1);





    globalcontext->insert(std::make_pair(RecursiveIterationExecutor::MEMEORYMANAGERS_key, new kfc(kfc{KPtr,{.p = (void *)&memorymgr}})));
    globalcontext->insert(std::make_pair(RecursiveIterationExecutor::THREADPOOLS_key,  new kfc(kfc{KPtr,{.p = (void *)&workers}})));
    globalcontext->insert(std::make_pair("root",new kfc(kfc{KS,{.s="SignalCalculation"}})));
 
    auto configs = buildtestconfig2("firstleveliterator");
 
    MultiLoopPlatform platform(configs.first, configs.second, globalcontext,"firstleveliterator",NUMTHREAD);
    platform.startCalc();
    return 0;
}