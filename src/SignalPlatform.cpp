#include "k.h"
#include "signalplatform.h"
#include <iostream>

using std::string;


RecursiveIterationExecutor::RecursiveIterationExecutor(ENV *env, ENVFunc *envf, KFCStore *creatervars, KFCStore *taskcontext, std::string componentname, int threadid ){
    this->env = env;
    this->ftable = envf;
    this->uppervars = creatervars;
    this->componentname = componentname;    
    this->taskcontext = taskcontext;
    // copy this variable space
    this->localvars = new KFCStore(*creatervars);
    this->threadid = threadid;
    this->logfile = std::fstream(std::string("logs/thread") + std::to_string(threadid) + std::string(".log"),std::ios::out | std::ios::app);
    
}


bool RecursiveIterationExecutor::preloop(){
    this->logfile << "calling preloop now" <<  std::endl;    
    MemoryManager *currentmgr = memmgrs->at(threadid);
    bool rtn = true;
    for (auto f: preloopfuncs)
    {
        rtn &= f.second(uppervars, localvars, taskcontext, currentmgr, logfile);
    }       
    return rtn;
}

bool RecursiveIterationExecutor::executeloop(){
    this->logfile << "calling executeloop now" << std::endl;
    KFC *currentchildtask = childtasks;


    if (execloopwithiter){

    if (usemultthread)
        this->logfile << "calling multithread execution" << std::endl;
    else
        this->logfile << "calling single thread execution" << std::endl;

    while( (*currentchildtask) != 0){
        if (usemultthread)
            threadpool->push([](int i, RecursiveIterationExecutor* exec, KFC currenttask ){
               auto child = exec->createChildIterator(currenttask, i);
               (*child)();
            }, this, *currentchildtask  );
        else{
             auto child = this->createChildIterator(*currentchildtask , threadid);
             (*child)();
        }
        currentchildtask ++;
    }

    if(usemultthread){
        threadpool->stop(true);
        this->logfile << "all threads completed" <<  std::endl;
    } else{
        this->logfile << "all tasks completed" <<  std::endl;
    }
    }
    else{
    MemoryManager *currentmgr = memmgrs->at(threadid);
    bool rtn = true;
    for (auto f: execloopfuncs)
    {
        this->logfile << "call " << f.first << "function" << std::endl;
        rtn &= f.second(uppervars, localvars, taskcontext, currentmgr,logfile);
    }       
    return rtn;

    }
}

bool RecursiveIterationExecutor::postloop(){
    this->logfile << "calling postloop now" << std::endl;    
    MemoryManager *currentmgr = memmgrs->at(threadid);
    bool rtn = true;
    for (auto f: postloopfuncs)
    {
        this->logfile << "call " << f.first << "function" << std::endl;
        rtn &= f.second(uppervars, localvars, taskcontext, currentmgr, logfile);
    }       
    return rtn;
}


bool RecursiveIterationExecutor::callgpu(){
    this->logfile << "calling callgpu now" << std::endl;    
    MemoryManager *currentmgr = memmgrs->at(threadid);
    bool rtn = true;
    for (auto f: gpufuncs)
    {
        this->logfile << "call " << f.first << "function" << std::endl;
        rtn &= f.second(uppervars, localvars, taskcontext, currentmgr, logfile);
    }       
    return rtn;
}

bool RecursiveIterationExecutor::joingpu(){
    this->logfile << "calling joingpu now" << std::endl;    
    return true;
}


RecursiveIterationExecutor * RecursiveIterationExecutor::createChildIterator(KFC childtask, int threadid){
        KFCStore * childcontext = new KFCStore(*taskcontext);
        childcontext->insert(std::make_pair(subitertaskname, childtask));
        RecursiveIterationExecutor * rtn = new RecursiveIterationExecutor(env, ftable, localvars, childcontext,subitercomponentname,threadid);
        this->logfile << "child iterator is created for task " << childtask  << std::endl;
        return rtn;
}



bool RecursiveIterationExecutor::inifromenv(ENV *env, ENVFunc *envf, KFCStore *context){
    this->logfile << "start the initialization" << std::endl;    
    bool rtn = true;
    // initialzie from configs
    rtn &= iniconfig(this->usemultthread, this->env, this->componentname,USEMULTITHREAD_key,0, KI, logfile);
    rtn &= iniconfig(this->execloopwithiter, this->env, this->componentname, USERECURSIVEINLOOP_key,0, KI ,logfile);
    rtn &= iniconfig(this->hasGPUtask, this->env, this->componentname, HASGPUTASK_key,0, KI ,logfile);
    rtn &= iniconfig(this->itertaskname, this->env, this->componentname, ITERATORTASK_key,0, KS ,logfile);
    if (execloopwithiter)
    {
        rtn &= iniconfig(this->subitercomponentname, this->env, this->componentname, SUBITERCOMPNAME_key,0, KS ,logfile);
        rtn &= iniconfig(this->subitertaskname, this->env, this->componentname, CHILDITERATORTASK_key,0, KS ,logfile);
    }
    if (!rtn)
        return rtn;
    // initialize from current context
    if (rtn &= !(0 == getvar(this->taskcontext, MEMEORYMANAGERS_key, logfile))){
        memmgrs = (MemoManagerStore *) getvar(this->taskcontext, MEMEORYMANAGERS_key, logfile)->p;
        this->logfile << MEMEORYMANAGERS_key <<  " is set" << std::endl;}
    
    if (rtn &= !(0 == getvar(this->taskcontext, THREADPOOLS_key, logfile))){
        threadpool = (ctpl::thread_pool *) getvar(this->taskcontext, THREADPOOLS_key, logfile)->p;
        this->logfile << THREADPOOLS_key <<  " is set" << std::endl;}

    if (rtn = !(0 == getvar(this->taskcontext, this->itertaskname, logfile))){
        this->currenttask =  getvar(this->taskcontext, this->itertaskname, logfile);
                this->logfile << "current task is set " <<  currenttask << std::endl;
    }

    if(rtn &= 0 != GETENVFuncf2(this->ftable, this->componentname,GETCHILDTASKS_key,0, logfile)){
        getchildtask =  GETENVFuncf2(this->ftable, this->componentname,GETCHILDTASKS_key,0, logfile);
        this->logfile << GETCHILDTASKS_key << "has loaded" << std::endl;
    }


    // load up functions from config
    if(0 != GETENVFuncMapf1(this->ftable, this->componentname, PRELOOPFUNC_key, logfile)){
        this->preloopfuncs = std::map<int, IterExecFunction> (*GETENVFuncMapf1(this->ftable, this->componentname, PRELOOPFUNC_key,logfile));
        this->logfile << PRELOOPFUNC_key << "has loaded " <<  this->preloopfuncs.size() << " functions"<< std::endl;
    }

    if(0 != GETENVFuncMapf1(this->ftable, this->componentname, POSTLOOPFUNC_key,logfile)){
        this->postloopfuncs = std::map<int, IterExecFunction> (*GETENVFuncMapf1(this->ftable, this->componentname, POSTLOOPFUNC_key, logfile));
        this->logfile << POSTLOOPFUNC_key << "has loaded " <<  this->postloopfuncs.size() << " functions"<< std::endl;
    }

    if(0 != GETENVFuncMapf1(this->ftable, this->componentname, EXECLOOPFUNC_key,logfile)){
        this->execloopfuncs = std::map<int, IterExecFunction> (*GETENVFuncMapf1(this->ftable, this->componentname, EXECLOOPFUNC_key,logfile));
        this->logfile << EXECLOOPFUNC_key << "has loaded " <<  this->execloopfuncs.size() << " functions"<< std::endl;
    }

    if(0 != GETENVFuncMapf1(this->ftable, this->componentname, GPUFUNC_key,logfile)){
        this->gpufuncs = std::map<int, IterExecFunction> (*GETENVFuncMapf1(this->ftable, this->componentname, GPUFUNC_key,logfile));
        this->logfile << GPUFUNC_key << "has loaded " <<  this->gpufuncs.size() << " functions";
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








