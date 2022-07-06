#ifndef SIGPLATFORM
#define SIGPLATFORM

#include <vector>
#include <map>
#include <string>
#include <fstream>


#include "ctpl_stl.h"
#include "k.h"      

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
class RecursiveIterationExecutor{

public:
enum loopexecmode {SingleThread, ThreadPool};

RecursiveIterationExecutor(ENV *env, ENVFunc *envf, KFCStore *creatervars, KFCStore *taskcontext, std::string componentname, int threadid );

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
        this->logfile << "ini failed \n";
        return INIFAILED;}
    bool prejob = preloop();
    childtasks = getchildtask(localvars);

    bool gpustate = true;
    if (hasGPUtask)
        gpustate = callgpu();
    bool job = executeloop();
    if (hasGPUtask)
        gpustate = joingpu();
    bool postjob = postloop();

    delete[] childtasks;
    logfile.flush();
    return NOERROR;
};
bool inifromenv(ENV* , ENVFunc*, KFCStore*);

template<typename T>
static bool iniconfig(T& config, ENV* env, std::string componentname, std::string attribute, int prior, int attributtype, std::iostream &out ){
    KFC configval;
    if (0 ==  (configval = GETENV(env,componentname,attribute,prior,out))){
        out << "<" << componentname << "|" << attribute << "|" << prior << "> config not found" << std::endl;
        return false;
    }
    switch (attributtype)
    {
    case KI:
        config = configval->i;
        out<< "config set " << componentname << "|" << attribute << "|" << prior <<  "> set to " << config << std::endl;
        break;
    case KS:
        config = configval->s;
        out << "config set " << componentname << "|" << attribute << "|" << prior <<  "> set to " << config << std::endl;
        break;
    default:
        break;
    }
    return true;
}

static KFC getvar(KFCStore *context, std::string varname, std::iostream &out){
    if(context->find(varname) == context->end()){
        out << "var " << varname << "not found " << std::endl;
        return 0;
    }
    return context -> at(varname);
}


RecursiveIterationExecutor *createChildIterator(KFC childtask, int threadid);



private:

ENV* env;
ENVFunc *ftable;
// current date, symbol, etc.
KFCStore *taskcontext;
// vars inherited from upper caller
KFCStore *uppervars;
int threadid;
CalculationLevel currentlevel;

// initialize from var, global tools
std::string componentname;
MemoManagerStore *memmgrs;
ctpl::thread_pool *threadpool;

// initialize from config
bool usemultthread;
bool execloopwithiter;
std::string subitercomponentname;
bool hasGPUtask;
std::string itertaskname;
std::string subitertaskname;



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

std::fstream logfile;


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

};



class MultiLoopPlatform: public BasSignalPlatform{

public:
MultiLoopPlatform(ENV *envx,ENVFunc *envfx,KFCStore *context ,std::string rootitername="root", int mainthreadid=999):
    env(envx), envf(envfx) ,globalvars(context) ,rootiterator(env,envf, new KFCStore(), globalvars ,rootitername, mainthreadid){
    ;
}

~MultiLoopPlatform(){
    delete globalvars;
}

bool startCalc(){
    rootiterator();
}

private:
ENV *env;
ENVFunc *envf;
KFCStore *globalvars;
RecursiveIterationExecutor rootiterator;

};






















#endif