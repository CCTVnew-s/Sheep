// This is the signal related variable API, such that creation/maintain is easier
// Think you're programming in a weak type way,
#ifndef VARIABLESTORE
#define VARIABLESTORE

#include <vector>
#include <map>
#include <tuple>
#include <string>
#include <iostream>
#include "k.h"

#include "consts.h"
#include "kutil.h"
#include "memorymanager.h"


// this is easy for c++ table, but how about cuda?
// need some reform for cuda, no std case
class tableview;



// don't need to be exactly the same as kdb-q

#define _KFCKBpair 1 , char  
#define _KFCKGpair 4 , char
#define _KFCKHpair 5 , short 
#define _KFCKIpair 6 , int   
#define _KFCKJpair 7 , long 
#define _KFCKEpair 8 , float
#define _KFCKFpair 9 , double 
#define _KFCKCpair 10, char
#define _KFCKSpair 11, char* 

#define _KFCKPpair 12, long
#define _KFCKMpair 13, int 
#define _KFCKDpair 14, int

#define _KFCKNpair 16, long
#define _KFCKUpair 17, int
#define _KFCKVpair 18, int
#define _KFCKTpair 19, int

#define _KFCKZpair 15, double
#define _KFCKPtrpair 30,void*
#define _KFCKTVpair  40,tablevew *

#define EXPAND_FUN_OVER_KTYPES(fun, ...) \
      fun(KBpair, __VA_ARGS__) \
      fun(KGpair, __VA_ARGS__) \
      fun(KHpair, __VA_ARGS__) \
      fun(KIpair, __VA_ARGS__) \
      fun(KJpair, __VA_ARGS__) \
      fun(KEpair, __VA_ARGS__) \
      fun(KFpair, __VA_ARGS__) \
      fun(KCpair, __VA_ARGS__) \
      fun(KSpair, __VA_ARGS__) \
      fun(KPpair, __VA_ARGS__) \
      fun(KMpair, __VA_ARGS__) \
      fun(KDpair, __VA_ARGS__) \
      fun(KNpair, __VA_ARGS__) \
      fun(KUpair, __VA_ARGS__) \
      fun(KVpair, __VA_ARGS__) \
      fun(KTpair, __VA_ARGS__)







// we need sth similar here, one class for every type, then it can be "stored"

typedef struct kfc{int t;union{H h;I i;J j;E e;F f;S s; void* p;tableview *k;struct{J n;void* G0;};};} *KFC;
#define EMPTYKFC 0

#define DECKFCBUILD(type)  KFC buildKFC(type, int);
DECKFCBUILD(H)
DECKFCBUILD(I)
DECKFCBUILD(J)
DECKFCBUILD(E)
DECKFCBUILD(F)
DECKFCBUILD(S)
DECKFCBUILD(void *)
DECKFCBUILD(tableview*)

template<typename T>
KFC buildsKFCs(T p, int type, int size){
    KFC rtn = new kfc();
    rtn->t = - type;
    rtn->n = size;
    rtn->G0 = (void*) p;
    return rtn;
};


std::ostream& operator<<(std::ostream& os, const KFC& f);

// currently support plain value; vanilla vectors; table slice
// currently use this, let's see any other actions need to be supported


typedef std::map<std::string, KFC>  KFCStore;

typedef bool (*IterExecFunction)(KFCStore *localvars, KFCStore *context, MemoryManagerSet &mgr, std::map<CalculationLevel, KFC> currentask);

typedef KFC* (*GetChildTaskFunc)(KFCStore *localvars, std::map<CalculationLevel, KFC> currentask);

struct funvariant {IterExecFunction f1; GetChildTaskFunc f2;} ;


#define EMPTYITER 0


// define <ComponnetName, Attribute, Priority, Value> as simple ENV class, environment table class
typedef std::map<std::string,std::map<std::string, std::map<int, KFC>*>*> ENV;
typedef std::map<std::string,std::map<std::string, std::map<int, funvariant>*>*> ENVFunc;



bool INSERTENV(ENV *t, std::string c, std::string a, int p, KFC val);
KFC GETENV(ENV *t, std::string c, std::string a, int p);
bool INSERTENVFunc(ENVFunc *t, std::string c, std::string a, int p, IterExecFunction val);
bool INSERTENVFunc(ENVFunc *t, std::string c, std::string a, int p, GetChildTaskFunc val);
IterExecFunction GETENVFuncf1(ENVFunc *t, std::string c, std::string a, int p);
GetChildTaskFunc GETENVFuncf2(ENVFunc *t, std::string c, std::string a, int p);

std::map<int, IterExecFunction> * GETENVFuncMapf1(ENVFunc *t, std::string c, std::string a);


#endif