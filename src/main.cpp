#include "variablestore.h"
#include "signalplatform.h"
#include "signalplatformtest.h"
#include <iostream>
#include <thread>
#include <cstring>


#define NUMTHREAD 5







int main(){

    KFCStore *globalcontext = new KFCStore();

    auto memorymgr = buildnmemorymgr(NUMTHREAD + 1,std::vector<CalculationLevel>{CalculationLevel::Top, CalculationLevel::Date, CalculationLevel::Signal},
    std::vector<MemoryLifeCyle>{MemoryLifeCyle::Global, MemoryLifeCyle::SinglePhase, MemoryLifeCyle::Task},1024L*1024L*1024L*2L);
    ctpl::thread_pool workers(NUMTHREAD);

    I datahandle = khpunc("localhost", 8939, "",1000, 1);





    // globalcontext->insert(std::make_pair(RecursiveIterationExecutor::MEMEORYMANAGERS_key, new kfc(kfc{KPtr,{.p = (void *)&memorymgr}})));
    // globalcontext->insert(std::make_pair(RecursiveIterationExecutor::THREADPOOLS_key,  new kfc(kfc{KPtr,{.p = (void *)&workers}})));
    globalcontext->insert(std::make_pair("root",new kfc(kfc{KS,{.s="SignalCalculation"}})));
 
    auto configs = buildtestconfig2("firstleveliterator");
 
    MultiLoopPlatform platform(configs.first, configs.second, globalcontext,"firstleveliterator",&memorymgr, &workers);
    platform.startCalc();
    return 0;
}