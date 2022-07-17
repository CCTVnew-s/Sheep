#include "k.h"
#include "signalplatform.h"
#include <iostream>

using std::string;


RecursiveIterationExecutor::RecursiveIterationExecutor(ENV *env, ENVFunc *envf, KFCStore *creatervars, KFCStore *taskcontext, std::string componentname, MemoManagerByThreads* mmmgrs, MemoryManager* curmmmgr, ctpl::thread_pool *threadpool,  std::map<CalculationLevel,KFC> currenttask){
    this->env = env;
    this->ftable = envf;
    this->uppervars = creatervars;
    this->componentname = componentname;    
    this->taskcontext = taskcontext;
    // copy this variable space
    this->localvars = new KFCStore(*creatervars);
    this->memmgrs = mmmgrs;
    this->curmemmgr = curmmmgr;
    this->threadpool = threadpool;
    // 
    this->currenttask = currenttask;
}


bool RecursiveIterationExecutor::preloop(){
    LOG(INFO,executepreloop, EXECUTORID<< "calling preloop now" <<  std::endl);    
    bool rtn = true;
    MemoryManagerSet& m = *(this->curmemmgr->getMemoryManagerSet(currentlevel));
    for (auto f: preloopfuncs)
    {
        rtn &= f.second( localvars, taskcontext, m, currenttask);
    }
    if (m.find(MemoryLifeCyle::SingleTaskPhase)==m.end())
        LOG(CRITIAL,executepreloop, EXECUTORID<< "no phase local memory, ignore reset\n");
    else{
        LOG(INFO,executepreloop, EXECUTORID<< "reset single phase local memory\n");
        m.at(MemoryLifeCyle::SingleTaskPhase)->reset();
    }
    return rtn;
}

bool RecursiveIterationExecutor::executeloop(){
    LOG(INFO,executepreloop,EXECUTORID << "calling executeloop now" << std::endl);
    KFC *currentchildtask = childtasks;

    bool rtn = true;
    MemoryManagerSet& m = *(this->curmemmgr->getMemoryManagerSet(currentlevel));

    if (execloopwithiter){

    if (usemultthread)
        LOG(INFO,executeloop,EXECUTORID << "calling multithread execution" << std::endl);
    else
        LOG(INFO,executeloop,EXECUTORID  << "calling single thread execution" << std::endl);

    while( (*currentchildtask) != 0){
        if (usemultthread)
            threadpool->push( [this](int i, RecursiveIterationExecutor* exec, KFC currenttask ){
               LogByThread::bundlecurrentidwithID(i);
               auto child = exec->createChildIterator(currenttask, this->memmgrs->at(i) );
               (*child)();
            }, this, *currentchildtask  );
        else{
             auto child = this->createChildIterator(*currentchildtask , this->curmemmgr);
             (*child)();
        }
        currentchildtask ++;
    }

    if(usemultthread){
        threadpool->stop(true);
        threadpool->reset();

        LOG(INFO,executeloop,EXECUTORID   << "all threads completed" <<  std::endl);
    } else{
        LOG(INFO,executeloop,EXECUTORID   << "all tasks completed" <<  std::endl);
    }
    }
    else{

    for (auto f: execloopfuncs)
    {
        LOG(INFO,executeloop,EXECUTORID   << "call " << f.first << "function" << std::endl);
        rtn &= f.second(localvars, taskcontext, m, currenttask);
    }
    }
    

    if (m.find(MemoryLifeCyle::SingleTaskPhase)==m.end())
        LOG(CRITIAL,executeloop, EXECUTORID<< "no phase local memory, ignore reset \n");
    else{
        LOG(INFO,executeloop, EXECUTORID<< "reset single phase local memory \n");
        m.at(MemoryLifeCyle::SingleTaskPhase)->reset();
    }
    return rtn;

}

bool RecursiveIterationExecutor::postloop(){
    LOG(INFO,executepostloop,EXECUTORID  << "calling postloop now" << std::endl);    
    bool rtn = true;
    MemoryManagerSet& m = *(this->curmemmgr->getMemoryManagerSet(currentlevel));

    for (auto f: postloopfuncs)
    {
         LOG(INFO,executepostloop,EXECUTORID << "call " << f.first << "function" << std::endl);
        rtn &= f.second(localvars, taskcontext, *(this->curmemmgr->getMemoryManagerSet(currentlevel)), currenttask);
    }
    if (m.find(MemoryLifeCyle::SingleTaskPhase)==m.end())
        LOG(CRITIAL,executepostloop, EXECUTORID<< "no phase local memory, ignore reset \n");
    else{
        LOG(INFO,executepostloop, EXECUTORID<< "reset single phase local memory \n");
        m.at(MemoryLifeCyle::SingleTaskPhase)->reset();
    }
    return rtn;
}


bool RecursiveIterationExecutor::callgpu(){
    LOG(INFO,executeGPU,EXECUTORID << "calling callgpu now" << std::endl);
    bool rtn = true;
    MemoryManagerSet& m = *(this->curmemmgr->getMemoryManagerSet(currentlevel));

    for (auto f: gpufuncs)
    {
        LOG(INFO,executeGPU,EXECUTORID << "call " << f.first << "function" << std::endl);
        rtn &= f.second(localvars, taskcontext,  *(this->curmemmgr->getMemoryManagerSet(currentlevel)), currenttask);
    }
    
    if (m.find(MemoryLifeCyle::SingleTaskPhase)==m.end())
        LOG(CRITIAL,executeGPU, EXECUTORID<< "no phase local memory, ignore reset \n");
    else{
        LOG(INFO,executeGPU, EXECUTORID<< "reset single phase local memory \n");
        m.at(MemoryLifeCyle::SingleTaskPhase)->reset();
    }
    return rtn;
}

bool RecursiveIterationExecutor::joingpu(){
    LOG(INFO,joinGPU,EXECUTORID << "calling joingpu now" << std::endl);
    return true;
}


RecursiveIterationExecutor * RecursiveIterationExecutor::createChildIterator(KFC childtask, MemoryManager *childmgr){
        KFCStore * childcontext = new KFCStore(*taskcontext);
        childcontext->insert(std::make_pair(subitertaskname, childtask));
        std::map<CalculationLevel, KFC> childtasktree(currenttask);
        childtasktree.insert(std::make_pair(subitertasklevel, childtask));
        RecursiveIterationExecutor * rtn = new RecursiveIterationExecutor(env, ftable, localvars, childcontext,subitercomponentname,this->memmgrs,childmgr, this->threadpool, childtasktree);
        LOG(INFO, EXECUTROCREATCHILDTASK, EXECUTORID << "child iterator is created for task " << childtask  << std::endl);
        return rtn; 
}



bool RecursiveIterationExecutor::inifromenv(ENV *env, ENVFunc *envf, KFCStore *context){
    LOG(INFO,EXECUTORINI, EXECUTORID<< "start the initialization" << std::endl);
    bool rtn = true;
    // initialzie from configs
    int levelconfig;
    rtn &= iniconfig(levelconfig, this->env, this->componentname, CALCULATIONLEVEL_key, 0);
    this->currentlevel = static_cast<CalculationLevel>(levelconfig);
    LOG(INFO,EXECUTORINI, EXECUTORID << CALCULATIONLEVEL_key << "has loaded at " << int(this->currentlevel)  << std::endl);

    rtn &= iniconfig(this->usemultthread, this->env, this->componentname,USEMULTITHREAD_key,0);
    rtn &= iniconfig(this->execloopwithiter, this->env, this->componentname, USERECURSIVEINLOOP_key,0 );
    rtn &= iniconfig(this->hasGPUtask, this->env, this->componentname, HASGPUTASK_key,0);
    rtn &= iniconfig(this->itertaskname, this->env, this->componentname, ITERATORTASK_key,0);
    

    // things need to know about your child, next name, next level, taskname --> legacy get task from context
    if (execloopwithiter)
    {
        rtn &= iniconfig(this->subitercomponentname, this->env, this->componentname, SUBITERCOMPNAME_key,0 );
        rtn &= iniconfig(this->subitertaskname, this->env, this->componentname, CHILDITERATORTASK_key,0 );
        int subiterlevel;
        rtn &= iniconfig(subiterlevel, this->env, this->componentname, SUBITERCALCULATIONLEVEL_key, 0);
        this->subitertasklevel = static_cast<CalculationLevel>(subiterlevel);
        bool valid = subitertasklevel > currentlevel;
        if (!valid)
                LOG(INFO,EXECUTORINI, EXECUTORID << "sub iter level "<< int(subitertasklevel) << " is not greater than current level " << int(currentlevel)   << std::endl);
        rtn &= valid;
    }
    if (!rtn)
        return rtn;
    
    
    // initialize from current context  ~~ not good
    // if (rtn &= !(0 == getvar(this->taskcontext, THREADPOOLS_key))){
    //     threadpool = (ctpl::thread_pool *) getvar(this->taskcontext, THREADPOOLS_key)->p;
    //    LOG(INFO,EXECUTORINI, EXECUTORID << THREADPOOLS_key <<  " is set" << std::endl);
    // }

    // if (rtn = !(0 == getvar(this->taskcontext, this->itertaskname))){
    //     this->currenttask =  getvar(this->taskcontext, this->itertaskname);
    //     LOG(INFO,EXECUTORINI, EXECUTORID << "current task is set " <<  currenttask << std::endl);
    // }

    // intitiate from config

    if(rtn &= 0 != GETENVFuncf2(this->ftable, this->componentname,GETCHILDTASKS_key,0)){
        getchildtask =  GETENVFuncf2(this->ftable, this->componentname,GETCHILDTASKS_key,0);
        LOG(INFO,EXECUTORINI, EXECUTORID << GETCHILDTASKS_key << "has loaded" << std::endl);
    }

    if(0 != GETENVFuncMapf1(this->ftable, this->componentname, PRELOOPFUNC_key)){
        this->preloopfuncs = std::map<int, IterExecFunction> (*GETENVFuncMapf1(this->ftable, this->componentname, PRELOOPFUNC_key));
        LOG(INFO,EXECUTORINI, EXECUTORID << PRELOOPFUNC_key << "has loaded " <<  this->preloopfuncs.size() << " functions"<< std::endl);
    }

    if(0 != GETENVFuncMapf1(this->ftable, this->componentname, POSTLOOPFUNC_key)){
        this->postloopfuncs = std::map<int, IterExecFunction> (*GETENVFuncMapf1(this->ftable, this->componentname, POSTLOOPFUNC_key));
        LOG(INFO,EXECUTORINI, EXECUTORID << POSTLOOPFUNC_key << "has loaded " <<  this->postloopfuncs.size() << " functions"<< std::endl);
    }

    if(0 != GETENVFuncMapf1(this->ftable, this->componentname, EXECLOOPFUNC_key)){
        this->execloopfuncs = std::map<int, IterExecFunction> (*GETENVFuncMapf1(this->ftable, this->componentname, EXECLOOPFUNC_key));
        LOG(INFO,EXECUTORINI, EXECUTORID << EXECLOOPFUNC_key << "has loaded " <<  this->execloopfuncs.size() << " functions"<< std::endl);
    }

    if(0 != GETENVFuncMapf1(this->ftable, this->componentname, GPUFUNC_key)){
        this->gpufuncs = std::map<int, IterExecFunction> (*GETENVFuncMapf1(this->ftable, this->componentname, GPUFUNC_key));
        LOG(INFO,EXECUTORINI, EXECUTORID << GPUFUNC_key << "has loaded " <<  this->gpufuncs.size() << " functions");
    }

    //    thread to apply class sub function

    
    return rtn;
}


const string RecursiveIterationExecutor::MEMEORYMANAGERS_key = "MemoryManagers";
const string RecursiveIterationExecutor::THREADPOOLS_key = "ThreadPools";
const string RecursiveIterationExecutor::USEMULTITHREAD_key = "UseMultiThread";
const string RecursiveIterationExecutor::USERECURSIVEINLOOP_key = "UseRecurveInLoop";
const string RecursiveIterationExecutor::SUBITERCOMPNAME_key = "SubIteratorComponentName";
const string RecursiveIterationExecutor::HASGPUTASK_key = "HasGPUTask";
const string RecursiveIterationExecutor::ITERATORTASK_key = "CurrentIteratorTask";
const string RecursiveIterationExecutor::CHILDITERATORTASK_key = "ChildIteratorTask";

const string RecursiveIterationExecutor::PRELOOPFUNC_key = "PRELOOPFUNCs";
const string RecursiveIterationExecutor::POSTLOOPFUNC_key = "POSTLOOPFUNCs";
const string RecursiveIterationExecutor::EXECLOOPFUNC_key = "EXECLOOPFUNCs";
const string RecursiveIterationExecutor::GPUFUNC_key = "GPUFUNCs";
const string RecursiveIterationExecutor::GETCHILDTASKS_key = "GetChildTasks";
const string RecursiveIterationExecutor::CALCULATIONLEVEL_key = "GetCalculationLevel";
const string RecursiveIterationExecutor::SUBITERCALCULATIONLEVEL_key = "GetSubIterCalculationLevel";








