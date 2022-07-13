#ifndef SIGPLATFORM
#define SIGPLATFORM

#include <vector>
#include <map>
#include <string>
#include <fstream>


#include "ctpl_stl.h"
#include "k.h"      
#include "logging.h"
#include "variablestore.h"




class BasSignalPlatform{
public:
bool  startCalc() {return false;};

};



// memory blocks, could be reused by one thread when doing multiple tasks
// could only help on arrays, large memory chunk wanted to be allocated
// suport a) tracking memory usage b) extend memory when it's not enough
// MainThread + TaskThread

// organize iteration of tasks, might take a recursive approach


#define EXECUTORID  " exectask " << currenttask << " :" 


class RecursiveIterationExecutor{

public:
enum loopexecmode {SingleThread, ThreadPool};

RecursiveIterationExecutor(ENV *env, ENVFunc *envf, KFCStore *creatervars, KFCStore *taskcontext, std::string componentname, MemoManagerByThreads* mmmgrs, MemoryManager* curmmmgr, ctpl::thread_pool *threadpool, KFC currenttask=EMPTYKFC);

// Intialize its own localvars
bool preloop();
bool executeloop();
bool postloop();
bool callgpu();
bool joingpu();

// DEFINE 
enum iterationerr {NOERROR=0, INIFAILED=1};

int operator()(){
    bool init = inifromenv(env, ftable, taskcontext);
    if (!init){
        LOG(ERROR, EXECUTORINITAIL, "ini failed \n");
        return INIFAILED;}
    MemoryManagerSet& m = *(this->curmemmgr->getMemoryManagerSet(currentlevel));

    bool prejob = preloop();
    childtasks = getchildtask(localvars);

    bool gpustate = true;
    if (hasGPUtask)
        gpustate = callgpu();
    bool job = executeloop();
    if (hasGPUtask)
        gpustate = joingpu();
    bool postjob = postloop();

    if (m.find(MemoryLifeCyle::Task)==m.end())
        LOG(CRITIAL,execute, EXECUTORID<< "no task level memory, ignore reset\n");
    else{
        LOG(INFO,execute, EXECUTORID<< "reset task level local memory\n");
        m.at(MemoryLifeCyle::Task)->reset();
    }    

    delete[] childtasks;
    LOGFLUSH;
    return NOERROR;
};
bool inifromenv(ENV* , ENVFunc*, KFCStore*);


#define INICONFIGTEMPLATE(type,typeid)                                                                                                           \
static bool iniconfig(type& config, ENV* env, std::string componentname, const std::string attribute, int prior){                                      \
    KFC configval;                                                                                                                               \
    if (0 ==  (configval = GETENV(env,componentname,attribute,prior))){                                                                          \
        LOG(ERROR, EXECUTORINICONFIG ,  "<" << componentname << "|" << attribute << "|" << prior << "> config not found" << std::endl);          \
        return false;                                                                                                                            \
    }                                                                                                                                            \
    config = configval->typeid;                                                                                                                  \
    LOG(DEBUG, EXECUTORINICONFIG,  "config set " << componentname << "|" << attribute << "|" << prior <<  "> set to " << config << std::endl);   \
    return true;}                                                                                                                                \

INICONFIGTEMPLATE(int,i)
INICONFIGTEMPLATE(std::string,s)
INICONFIGTEMPLATE(bool,i)




static KFC getvar(KFCStore *context, std::string varname){
    if(context->find(varname) == context->end()){
        LOG(ERROR, EXECUTORGETVAR,  "var " << varname << "not found " << std::endl);
        return 0;
    }
    return context -> at(varname);
}


RecursiveIterationExecutor *createChildIterator(KFC childtask, MemoryManager *childmgr);



private:

ENV* env;
ENVFunc *ftable;
// current date, symbol, etc.
KFCStore *taskcontext;
// vars inherited from upper caller
KFCStore *uppervars;


// initialize from var, global tools
std::string componentname;
MemoManagerByThreads *memmgrs; // this is just for building sub purpose
MemoryManager *curmemmgr;
ctpl::thread_pool *threadpool;

// initialize from config
bool usemultthread;
bool execloopwithiter;
std::string subitercomponentname;
bool hasGPUtask;
std::string itertaskname;
std::string subitertaskname;
CalculationLevel currentlevel;



std::map<int, IterExecFunction> preloopfuncs;
std::map<int, IterExecFunction> execloopfuncs;
std::map<int, IterExecFunction> postloopfuncs;
std::map<int, IterExecFunction> gpufuncs;


// initialize from configured func
// vars created for this loop
KFCStore *localvars;
KFC currenttask;
KFC* childtasks;
GetChildTaskFunc getchildtask;



public:
// config loopup keys

static const std::string MEMEORYMANAGERS_key;
static const std::string THREADPOOLS_key;
static const std::string USEMULTITHREAD_key;
static const std::string USERECURSIVEINLOOP_key;
static const std::string SUBITERCOMPNAME_key;
static const std::string HASGPUTASK_key;
static const std::string PRELOOPFUNC_key;
static const std::string POSTLOOPFUNC_key;
static const std::string EXECLOOPFUNC_key;
static const std::string GPUFUNC_key;
static const std::string ITERATORTASK_key;
static const std::string CHILDITERATORTASK_key;
static const std::string GETCHILDTASKS_key;
static const std::string CALCULATIONLEVEL_key;


};



class MultiLoopPlatform: public BasSignalPlatform{

public:
MultiLoopPlatform(ENV *envx,ENVFunc *envfx,KFCStore *context ,std::string rootitername, MemoManagerByThreads* mmmgrs, ctpl::thread_pool *threadpool, KFC currenttask = new kfc(kfc {KS, {.s = "ExecRootTask"}})):
    env(envx), envf(envfx) ,globalvarsandtask(context) ,rootiterator(env,envf, new KFCStore(), globalvarsandtask ,rootitername, mmmgrs, mmmgrs->at(mmmgrs->size() - 1),threadpool, currenttask){
}

~MultiLoopPlatform(){
    delete globalvarsandtask;
}

bool startCalc(){
    rootiterator();
}

private:
ENV *env;
ENVFunc *envf;
KFCStore *globalvarsandtask;
RecursiveIterationExecutor rootiterator;

};






















#endif