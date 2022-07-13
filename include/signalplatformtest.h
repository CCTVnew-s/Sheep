

#include "variablestore.h"
#include "signalplatform.h"
#include <iostream>
#include <thread>
#include <cstring>



 bool sample( KFCStore *localvars, KFCStore *context, MemoryManagerSet &mgr){
    LOGAndCOUT(INFO, SAMPLEF1, "calll sample function with id" << std::this_thread::get_id() << std::endl);
    LOGAndCOUT(INFO, SAMPLEF1, "this is from thread" <<   std::this_thread::get_id() << std::endl);
     return true;
 };


 KFC* samplegettasks(KFCStore *localvars){
     KFC *rtn = new KFC[21];
     for(int ix=0;ix<21;ix++)
        rtn[ix] = new kfc(kfc {KI, {.i = ix}}) ;
    rtn[20] = 0;
    return rtn; 
 };

 
#define MACROBUILDSAMPLEFUN(id) ([]( KFCStore *localvars, KFCStore *context, MemoryManagerSet &mgr){ return sample(localvars,context, mgr);})


std::pair<ENV*, ENVFunc*> buildtestconfig(std::string component1){
    ENV* test1config = new ENV();
    ENVFunc* testconfigf = new ENVFunc();
    // 

    INSERTENV(test1config,component1,RecursiveIterationExecutor::USEMULTITHREAD_key, 0 ,buildKFC(0,KI)   );
    INSERTENV(test1config,component1,RecursiveIterationExecutor::USERECURSIVEINLOOP_key, 0 ,buildKFC(0,KI)  );
    INSERTENV(test1config,component1,RecursiveIterationExecutor::HASGPUTASK_key, 0 ,buildKFC(0,KI) );
    INSERTENV(test1config,component1,RecursiveIterationExecutor::ITERATORTASK_key, 0 ,new kfc(kfc {KS, {.s = "root"}}));
    INSERTENV(test1config,component1,RecursiveIterationExecutor::SUBITERCOMPNAME_key, 0 , new kfc(kfc{KS, {.s="DateIter"}}));
    INSERTENV(test1config,component1,RecursiveIterationExecutor::CHILDITERATORTASK_key, 0 ,new kfc(kfc{KS, {.s="Date"}}));
    
    INSERTENVFunc(testconfigf, component1, RecursiveIterationExecutor::GETCHILDTASKS_key,0, samplegettasks);
    INSERTENVFunc(testconfigf, component1, RecursiveIterationExecutor::PRELOOPFUNC_key,0, MACROBUILDSAMPLEFUN("call 1st preloop"));
    INSERTENVFunc(testconfigf, component1, RecursiveIterationExecutor::PRELOOPFUNC_key,1, MACROBUILDSAMPLEFUN("call 2nd preloop"));
    INSERTENVFunc(testconfigf, component1, RecursiveIterationExecutor::POSTLOOPFUNC_key,0, MACROBUILDSAMPLEFUN("call 1st postloop"));
    INSERTENVFunc(testconfigf, component1, RecursiveIterationExecutor::POSTLOOPFUNC_key,1, MACROBUILDSAMPLEFUN("call 2nd postloop"));
    INSERTENVFunc(testconfigf, component1, RecursiveIterationExecutor::EXECLOOPFUNC_key,0, MACROBUILDSAMPLEFUN("call 1st looping"));
    INSERTENVFunc(testconfigf, component1, RecursiveIterationExecutor::EXECLOOPFUNC_key,1, MACROBUILDSAMPLEFUN("call 12nd looping"));

    return std::make_pair(test1config,testconfigf);
}



std::pair<ENV*, ENVFunc*> buildtestconfig2(std::string component1){
    ENV* test1config = new ENV();
    ENVFunc* testconfigf = new ENVFunc();
    // 
    INSERTENV(test1config,component1,RecursiveIterationExecutor::CALCULATIONLEVEL_key, 0 ,buildKFC(0,KI)   );
    INSERTENV(test1config,component1,RecursiveIterationExecutor::USEMULTITHREAD_key, 0 ,buildKFC(1,KI)   );
    INSERTENV(test1config,component1,RecursiveIterationExecutor::USERECURSIVEINLOOP_key, 0 ,buildKFC(1,KI)   );
    INSERTENV(test1config,component1,RecursiveIterationExecutor::HASGPUTASK_key, 0 ,buildKFC(0,KI)   );
    INSERTENV(test1config,component1,RecursiveIterationExecutor::ITERATORTASK_key, 0 ,new kfc(kfc {KS, {.s = "root"}}));
    INSERTENV(test1config,component1,RecursiveIterationExecutor::SUBITERCOMPNAME_key, 0 , new kfc(kfc{KS, {.s="DateIter"}}));
    INSERTENV(test1config,component1,RecursiveIterationExecutor::CHILDITERATORTASK_key, 0 ,new kfc(kfc{KS, {.s="Date"}}));
    

    INSERTENVFunc(testconfigf, component1, RecursiveIterationExecutor::GETCHILDTASKS_key,0, samplegettasks);
    INSERTENVFunc(testconfigf, component1, RecursiveIterationExecutor::PRELOOPFUNC_key,0, MACROBUILDSAMPLEFUN("call 1st preloop"));
    INSERTENVFunc(testconfigf, component1, RecursiveIterationExecutor::PRELOOPFUNC_key,1, MACROBUILDSAMPLEFUN("call 2nd preloop"));
    INSERTENVFunc(testconfigf, component1, RecursiveIterationExecutor::POSTLOOPFUNC_key,0, MACROBUILDSAMPLEFUN("call 1st postloop"));
    INSERTENVFunc(testconfigf, component1, RecursiveIterationExecutor::POSTLOOPFUNC_key,1, MACROBUILDSAMPLEFUN("call 2nd postloop"));
    INSERTENVFunc(testconfigf, component1, RecursiveIterationExecutor::EXECLOOPFUNC_key,0, MACROBUILDSAMPLEFUN("call 1st looping"));
    INSERTENVFunc(testconfigf, component1, RecursiveIterationExecutor::EXECLOOPFUNC_key,1, MACROBUILDSAMPLEFUN("call 12nd looping"));


    std::string component2 = "DateIter";

    INSERTENV(test1config,component2,RecursiveIterationExecutor::CALCULATIONLEVEL_key, 0 ,buildKFC(100,KI)   );
    INSERTENV(test1config,component2,RecursiveIterationExecutor::USEMULTITHREAD_key, 0 ,buildKFC(0,KI)   );
    INSERTENV(test1config,component2,RecursiveIterationExecutor::USERECURSIVEINLOOP_key, 0 ,buildKFC(0,KI)   );
    INSERTENV(test1config,component2,RecursiveIterationExecutor::HASGPUTASK_key, 0 ,buildKFC(0,KI)   );

    INSERTENV(test1config,component2,RecursiveIterationExecutor::ITERATORTASK_key, 0 ,new kfc(kfc {KS, {.s = "Date"}}));
    INSERTENV(test1config,component2,RecursiveIterationExecutor::SUBITERCOMPNAME_key, 0 , new kfc(kfc{KS, {.s="SymbolIter"}}));
    INSERTENV(test1config,component2,RecursiveIterationExecutor::CHILDITERATORTASK_key, 0 ,new kfc(kfc{KS, {.s="Symbol"}}));
    
    INSERTENVFunc(testconfigf, component2, RecursiveIterationExecutor::GETCHILDTASKS_key,0, samplegettasks);
    INSERTENVFunc(testconfigf, component2, RecursiveIterationExecutor::PRELOOPFUNC_key,0, MACROBUILDSAMPLEFUN("call 1st preloop"));
    INSERTENVFunc(testconfigf, component2, RecursiveIterationExecutor::PRELOOPFUNC_key,1, MACROBUILDSAMPLEFUN("call 2nd preloop"));
    INSERTENVFunc(testconfigf, component2, RecursiveIterationExecutor::POSTLOOPFUNC_key,0, MACROBUILDSAMPLEFUN("call 1st postloop"));
    INSERTENVFunc(testconfigf, component2, RecursiveIterationExecutor::POSTLOOPFUNC_key,1, MACROBUILDSAMPLEFUN("call 2nd postloop"));
    INSERTENVFunc(testconfigf, component2, RecursiveIterationExecutor::EXECLOOPFUNC_key,0, MACROBUILDSAMPLEFUN("call 1st looping"));
    INSERTENVFunc(testconfigf, component2, RecursiveIterationExecutor::EXECLOOPFUNC_key,1, MACROBUILDSAMPLEFUN("call 12nd looping"));


    return std::make_pair(test1config,testconfigf);
}
