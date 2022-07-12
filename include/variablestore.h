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



std::iostream& operator<<(std::iostream& os, const KFC& f);

// currently support plain value; vanilla vectors; table slice
// currently use this, let's see any other actions need to be supported




typedef std::map<std::string, KFC>  KFCStore;

typedef bool (*IterExecFunction)(KFCStore *localvars, KFCStore *context, MemoryManagerSet &mgr);

typedef KFC* (*GetChildTaskFunc)(KFCStore *localvars);

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