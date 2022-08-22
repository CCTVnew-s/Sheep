/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   arraycalc.h
 * Author: xiez
 *
 * Created on August 11, 2022, 7:53 AM
 */

#ifndef ARRAYCALC_H
#define ARRAYCALC_H

#include <vector>
#include "memorymanager.h"
#include "logging.h"

template <typename T>
struct var{

var(const var &x):p(x.p),l(x.l){};
var():p(nullptr),l(0){};
var(T& x):p(&x),l(1){};
var(T* x,int lx): p(x),l(lx){};
T& operator[](int i){return l==1?p[0]:p[i];};
T* p;
int l;
int unitsize(){return sizeof(T);};
};


#define DECLAIRETYPEBUILD(type)  var<type> buildvar(type&,int);var<int> buildvar(type*,int);
DECLAIRETYPEBUILD(bool)
DECLAIRETYPEBUILD(int)
DECLAIRETYPEBUILD(short)
DECLAIRETYPEBUILD(char)
DECLAIRETYPEBUILD(long)
DECLAIRETYPEBUILD(double)
DECLAIRETYPEBUILD(float)


// I vectorize simple function to vec function
#define expandf1(f,l,y,x1) \
for(int i=0;i++;i<l) \
    f(buildvar(y,l)[i],buildvar(x1,l)[i]);

#define expandf2(f,l,y,x1,x2) \
for(int i=0;i++;i<l) \
    f(buildvar(y,l)[i],buildvar(x1,l)[i],buildvar(x2,l)[i]);

#define expand3(f,l,y,x1,x2,x3) \
for(int i=0;i++;i<l) \
    f(buildvar(y,l)[i],buildvar(x1,l)[i],buildvar(x2,l)[i],buildvar(x3,l)[i]);

#define expand4(f,l,y,x1,x2,x3,x4) \
for(int i=0;i++;i<l) \
    f(buildvar(y,l)[i],buildvar(x1,l)[i],buildvar(x2,l)[i],buildvar(x3,l)[i],buildvar(x4,l)[i]);

#define expand5(f,l,y,x1,x2,x3,x4,x5) \
for(int i=0;i++;i<l) \
    f(buildvar(y,l)[i],buildvar(x1,l)[i],buildvar(x2,l)[i],buildvar(x3,l)[i],buildvar(x4,l)[i],buildvar(x5,l)[i]);

#define expand6(f,l,y,x1,x2,x3,x4,x5,x6) \
for(int i=0;i++;i<l) \
    f(buildvar(y,l)[i],buildvar(x1,l)[i],buildvar(x2,l)[i],buildvar(x3,l)[i],buildvar(x4,l)[i],buildvar(x5,l)[i],buildvar(x6,l)[i]);

#define expand7(f,l,y,x1,x2,x3,x4,x5,x6,x7) \
for(int i=0;i++;i<l) \
    f(buildvar(y,l)[i],buildvar(x1,l)[i],buildvar(x2,l)[i],buildvar(x3,l)[i],buildvar(x4,l)[i],buildvar(x5,l)[i],buildvar(x6,l)[i],buildvar(x7,l)[i]);

#define expand8(f,l,y,x1,x2,x3,x4,x5,x6,x7,x8) \
for(int i=0;i++;i<l) \
    f(buildvar(y,l)[i],buildvar(x1,l)[i],buildvar(x2,l)[i],buildvar(x3,l)[i],buildvar(x4,l)[i],buildvar(x5,l)[i],buildvar(x6,l)[i],buildvar(x7,l)[i],buildvar(x8,l)[i]);

#define expand9(f,l,y,x1,x2,x3,x4,x5,x6,x7,x8,x9) \
for(int i=0;i++;i<l) \
    f(buildvar(y,l)[i],buildvar(x1,l)[i],buildvar(x2,l)[i],buildvar(x3,l)[i],buildvar(x4,l)[i],buildvar(x5,l)[i],buildvar(x6,l)[i],buildvar(x7,l)[i],buildvar(x8,l)[i],buildvar(x9,l)[i]);

// II standrad operators

#define EXPANDVARXCONVERTER(type) operator var<type>() {return var<type>((type*)p, l);};
#define EXPANDVARXFROMVAR(type) varx(const var<type> &v):p((void*)v.p),l(v.l){};

class varx{
public:

varx():p(nullptr),l(0){}; // should be invalid

varx(const varx &v):p(v.p),l(v.l){};
template <typename T>
varx(T& x):p((void*)&x),l(1){};
template <typename T>
varx(T* x,int lx): p((void*)x),l(lx){};

EXPANDVARXFROMVAR(bool)
EXPANDVARXFROMVAR(int)
EXPANDVARXFROMVAR(short)
EXPANDVARXFROMVAR(char)
EXPANDVARXFROMVAR(long)
EXPANDVARXFROMVAR(double)
EXPANDVARXFROMVAR(float)

int unitsize(){return 0;};

EXPANDVARXCONVERTER(bool)
EXPANDVARXCONVERTER(int)
EXPANDVARXCONVERTER(short)
EXPANDVARXCONVERTER(char)
EXPANDVARXCONVERTER(long)
EXPANDVARXCONVERTER(double)
EXPANDVARXCONVERTER(float)

operator bool() const {return p!=nullptr;}

void* p;
int l;
};




// may not be super useful for simple vector calculation, but used for chained computations
class stackvecopr{
public:
stackvecopr(int n):argn(n), args(std::vector<varx>()){};

virtual varx evaluate(MemoryManagerSingle* mem){
    // memory creation
    if(args.size()!=argn)
        return varx();
    auto rtn = evaluate_imp(mem);
    args.clear(); // clean, just evaluate once
    return rtn;
};
// modify return
virtual varx evaluate_imp(MemoryManagerSingle* mem) = 0;

virtual long unitrtnsize() = 0;


virtual bool regarg(varx a){
    args.push_back(a);
};




int argn;
std::vector<varx> args;
int resultsize;

};

#define MACRO_CAT(x, y)     x##y
#define MACRO_GLUE(x, y)    MACRO_CAT(x, y)

/*
    Automate repetitive types of content
*/
#define REPEAT_0(head, body)
#define REPEAT_1(head, body)     head(1)
#define REPEAT_2(head, body)     REPEAT_1(head, body)body(2)
#define REPEAT_3(head, body)     REPEAT_2(head, body)body(3)
#define REPEAT_4(head, body)     REPEAT_3(head, body)body(4)
#define REPEAT_5(head, body)     REPEAT_4(head, body)body(5)
#define REPEAT_6(head, body)     REPEAT_5(head, body)body(6)
#define REPEAT_7(head, body)     REPEAT_6(head, body)body(7)
#define REPEAT_8(head, body)     REPEAT_7(head, body)body(8)
#define REPEAT_9(head, body)     REPEAT_8(head, body)body(9)
#define REPEAT(n,head, body)     MACRO_GLUE(REPEAT_, n)(head, body)


#define HEADEREXP_H(n) varx& x##n
#define HEADEREXP_B(n) ,varx& x##n
#define HEADEREXP_N(n) REPEAT(n, HEADEREXP_H, HEADEREXP_B)

#define ARGSATX(x) args.at(x)
#define COMMA ,
#define VECARGEXP_H(n) ARGSATX(n)
#define VECARGEXP_B(n) ,ARGSATX(n)
#define VECARGEXP_N(n) REPEAT(n, VECARGEXP_H, VECARGEXP_B)



 // where operator?, these standard operators, could be some standard one, but also extend the interface (or )
 // n < 9
 // first argument is the output
 #define VECOPERATORN(n) \
 class vecopr_##n:public stackvecopr{public:    \
     vecopr_##n():stackvecopr(n){};      \
     virtual varx evaluatenotype(HEADEREXP_N(n),MemoryManagerSingle* mem ){return varx();} ;\
     virtual varx evaluate_imp(MemoryManagerSingle* mem){        \
        return evaluatenotype(VECARGEXP_N(n),mem); };      \
     virtual int returnlen(HEADEREXP_N(n)){return x1.l;};         \
 }; 

VECOPERATORN(1)
VECOPERATORN(2)
VECOPERATORN(3)
VECOPERATORN(4)
VECOPERATORN(5)
VECOPERATORN(6) 
VECOPERATORN(7)
VECOPERATORN(8)
VECOPERATORN(9)

#define SIMPLEXI_H(n) x##n
#define SIMPLEXI_B(n) ,x##n
#define SIMPLEXIEXP_N(n) REPEAT(n, SIMPLEXI_H, SIMPLEXI_B)

#define EXTENDOPERATOR(n) virtual varx evaluatenotype(HEADEREXP_N(n),MemoryManagerSingle* mem){decltype(this->operator()(varx(),SIMPLEXIEXP_N(n))) y; y.p = static_cast<decltype(y.p)>(mem->allocate(y.unitsize()*rtnsingle?1:returnlen(SIMPLEXIEXP_N(n))));y.l= rtnsingle?1:returnlen(SIMPLEXIEXP_N(n)); auto z = this->operator()(y,SIMPLEXIEXP_N(n));return z;};


// varx can be casted to/from any pointer with length

// we try to simplify operator, functions and methodoloy
// 1. Need to preallocate the space for result, rtn->length can be reduced , with temparariry memory leakage
// 2. It wanted to supoort chained calculation in CalcStack, it needs to provide information e.g. what's the return type (size) and dimonsion for intermidate results
// 3. If calculation failed, rtn won't be the same as argument rtn, such case, need to handle, especially we can't recollect the space allocated
// 4. whether we support inplace updates ??? seems okay, when you code the operators carefully
// 5. how to easy extend function into vector function,,, This is actually wraping f function with adverb,,parallel, scan, over??





class where:public vecopr_1{
public:
    // preallocated, if worked fine, var<int> is latest one, with updates, one issue here, "returned rtn" and original "rtn", if calculation failed, how you deal with memory?
    // return result type need to support y.unitsize(), available for all var templates
    var<int> operator()(var<int> rtn, var<bool> cond){
        int totaln = 0;
        for(int i=0;i<cond.l;i++)
            if (cond[i])
                {
                    rtn[totaln] = i;
                    totaln ++;
                }    
        rtn.l = totaln;
        return rtn;
    };
    static const bool rtnsingle=false;
    // virtual int returnlen(varx x1){return x1.l;}; trying to support sth more complicated, seems not enough
    // it should take the form of returnlen(x1,x2,,,,xn) with default 1 or x1.l 
    // need a special size function, can be ignored generally
    EXTENDOPERATOR(1)
    
};

// KEYS of implementation and usage
// 1) implement operator() as above, 
// 2) rtnsingle whether is single value rtn , if not x1 has the same length -- this need to be revisited, x1 should control loop, but 
//      you need some better way determine the return size
// 3) another way of usage  var<T> =  evaluatenotype(x1,x2,,,,xn, mem), mem will take care of the mem, meanwhile you lost memory when sth failed




#endif /* ARRAYCALC_H */
