// This is the signal related variable API, such that creation/maintain is easier
// Think you're programming in a weak type way,
#ifndef VARIABLESTORE
#define VARIABLESTORE

#include "k.h"
#include <vector>
#include <map>
#include <tuple>
#include <string>
#include <iostream>
#include "kutil.h"


// this is easy for c++ table, but how about cuda?
// need some reform for cuda, no std case
class tableview;

enum class CalculationLevel {Top ,Date, Symbol, SnapTime, Signal, Unknown}; //  Signal Level shouldn't be used, at Date level, means you're working on certain date e.g. 2021.01.01, you will loop over symbols of the date

enum class MemoryLifeCyle{Global, Task, SinglePhase};



class MemoryManager;
typedef std::map<int, MemoryManager*> MemoManagerByThreads;

class MemoryManagerSingle{
public:

    MemoryManagerSingle( long s){
        mem = malloc(s * sizeof(char));
        cursor = mem;
        sizebytes = s;
        used = 0;
        overfillmem = std::vector<void *> ();
    }

    void reset(){
        used = 0;
        cursor = mem;
        while(overfillmem.size()>0){
            free(overfillmem.back());
            overfillmem.pop_back();
        }
    }


    void * allocate(long size){
        if (sizebytes - used > size){
            void *memtmp = malloc(size* sizeof(char));
            overfillmem.push_back(memtmp);
            return memtmp;
        }
        else{
            used = used + size;
            void *rtn = cursor;
            cursor = (void *)((char *)cursor + size);
            return rtn;
        }
    }

    // issue is that, we need to update every pointer, who refer here
    void resize(long long s){
        if(sizebytes > 0 && sizebytes < s){
            ; // can't be supported for now
        }
        else if( sizebytes > 0 && sizebytes >= s) {
            return;
        }
        else{
            mem = malloc(s);
            sizebytes = s;
            used = 0;
        }
    }

private:
void *mem;
void *cursor;
long sizebytes;
long used;
std::vector<void *> overfillmem;

};


class MemoryManager{
public:
    std::map<std::pair<CalculationLevel, MemoryLifeCyle>, MemoryManagerSingle*> childmgrs;

    MemoryManager(std::map<std::pair<CalculationLevel, MemoryLifeCyle>, long> sizeconfig){
        childmgrs = std::map<std::pair<CalculationLevel, MemoryLifeCyle>, MemoryManagerSingle*>();
        for (auto mconfig: sizeconfig)
            childmgrs.insert(std::make_pair(mconfig.first, new  MemoryManagerSingle(mconfig.second)));
    }

    MemoryManagerSingle* getChildMgr(CalculationLevel l, MemoryLifeCyle c){
        if (childmgrs.find(std::make_pair(l,c)) == childmgrs.end())
            return NULL;
        else
            return childmgrs.at(std::make_pair(l,c));
    }
    

};


MemoManagerByThreads buildnmemorymgr(int n, std::map<std::pair<CalculationLevel, MemoryLifeCyle>, long> sizeconfig){
        MemoManagerByThreads rtn;
        for(int i=0;i<n;i++)
            rtn.insert(std::pair<int,MemoryManager*>(i,new MemoryManager(sizeconfig)));
        return rtn;
};



/*
These should automatically be supported for below
#define kG(x)	((x)->G0)
#define kC(x)	kG(x)
#define kH(x)	((H*)kG(x))
#define kI(x)	((I*)kG(x))
#define kJ(x)	((J*)kG(x))
#define kE(x)	((E*)kG(x))
#define kF(x)	((F*)kG(x))
#define kS(x)	((S*)kG(x))
#define kK(x)	((K*)kG(x))
*/
// we need sth similar here, one class for every type, then it can be "stored"
typedef struct kfc{int t;union{H h;I i;J j;E e;F f;S s;void* p;tableview *k;struct{J n;void* G0;};};} *KFC;
#define EMPTYKFC 0

// type def define
#define KPtr 30



std::ostream& operator<<(std::ostream& os, const KFC& f);


// currently support plain value; vanilla vectors; table slice

// currently use this, let's see any other actions need to be supported




typedef std::map<std::string, KFC>  KFCStore;

typedef bool (*IterExecFunction)(KFCStore* outervars, KFCStore *localvars, KFCStore *context, MemoryManager *mgr, std::iostream& out);

typedef KFC* (*GetChildTaskFunc)(KFCStore *localvars);

struct funvariant {IterExecFunction f1; GetChildTaskFunc f2;} ;


#define EMPTYITER 0


// define <ComponnetName, Attribute, Priority, Value> as simple ENV class, environment table class
typedef std::map<std::string,std::map<std::string, std::map<int, KFC>*>*> ENV;
typedef std::map<std::string,std::map<std::string, std::map<int, funvariant>*>*> ENVFunc;



bool INSERTENV(ENV *t, std::string c, std::string a, int p, KFC val);
KFC GETENV(ENV *t, std::string c, std::string a, int p, std::iostream& out);
bool INSERTENVFunc(ENVFunc *t, std::string c, std::string a, int p, IterExecFunction val);
bool INSERTENVFunc(ENVFunc *t, std::string c, std::string a, int p, GetChildTaskFunc val);
IterExecFunction GETENVFuncf1(ENVFunc *t, std::string c, std::string a, int p,std::iostream& out);
GetChildTaskFunc GETENVFuncf2(ENVFunc *t, std::string c, std::string a, int p,std::iostream& out);

std::map<int, IterExecFunction> * GETENVFuncMapf1(ENVFunc *t, std::string c, std::string a,std::iostream& out);




#endif