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
#include <list>
#include "memorymanager.h"
#include "logging.h"
#include <cstring>
#include "k.h"

namespace ARRAY{


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


#define DECLAIRETYPEBUILD(type)  var<type> buildvar(type&,int);var<type> buildvar(type*,int);
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

#define EXPANDVARXCONVERTER(type) operator var<type>() {return var<type>((type*)p, l) ;};
#define EXPANDVARXFROMVAR(type) varx(const var<type> &v):p((void*)v.p),l(v.l),unitsize(sizeof(type)){};

class varx{
public:

varx():p(nullptr),l(0),unitsize(0){}; // should be invalid

varx(const varx &v):p(v.p),l(v.l),unitsize(v.unitsize){};

template <typename T>
varx(T* x,int lx): p((void*)x),l(lx),unitsize(sizeof(T)){};

EXPANDVARXFROMVAR(bool)
EXPANDVARXFROMVAR(int)
EXPANDVARXFROMVAR(short)
EXPANDVARXFROMVAR(char)
EXPANDVARXFROMVAR(long)
EXPANDVARXFROMVAR(double)
EXPANDVARXFROMVAR(float)
EXPANDVARXFROMVAR(char*)


EXPANDVARXCONVERTER(bool)
EXPANDVARXCONVERTER(int)
EXPANDVARXCONVERTER(short)
EXPANDVARXCONVERTER(char)
EXPANDVARXCONVERTER(long)
EXPANDVARXCONVERTER(double)
EXPANDVARXCONVERTER(float)
EXPANDVARXCONVERTER(char*)


operator bool() const {return p!=nullptr;}

void* p;
int l;
int unitsize;
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


virtual bool regarg(varx a){
    args.push_back(a);
};

int argn;
std::vector<varx> args;
int resultsize;

};

// functions classes to support chained vector calculations

class calstackele{
public:

template <class T>
calstackele(var<T> oprdx):oprd(varx(oprdx)),opr(nullptr),isoperator(false){};
calstackele(varx oprdx):oprd(oprdx),opr(nullptr),isoperator(false){};
calstackele(stackvecopr *oprx):oprd(varx()),opr(oprx),isoperator(true){};

bool isoperator;
varx oprd;
stackvecopr* opr;
};


class calcstack{
public:

calcstack(calstackele ele){
    _lists = std::list<calstackele>();
    _lists.push_back(ele);
};

calcstack operator+(calstackele ele){
    calcstack rtn(*this);
    rtn._lists.push_back(ele);
    return rtn;
};

calcstack operator+(calcstack second){
    calcstack rtn(*this);
    for(auto x:second._lists)
        rtn._lists.push_back(x);
    return rtn;
};

varx evaluate(MemoryManagerSingle* draftmem,MemoryManagerSingle* rtnmem){
    // this mem should be special to calstack as draft paper
    draftmem->reset(); // find a new paper

    bool failed = false;
    // better take some iteration approach
    while(_lists.size()>1){
        if(!evaluateonce(draftmem))
        {
            LOG(ERROR,VECCALSTACK,"evaluation failed in th middle" << std::endl);
            failed = true;
            break;
        }
    }

    if(failed || _lists.size()!=1 || _lists.front().isoperator)
        return varx();
    varx rtn =  _lists.front().oprd;
    // need to copy this baby to a permanent place;
    void* copyto = rtnmem->allocate(rtn.l * rtn.unitsize);
    std::memcpy(copyto, rtn.p, rtn.l * rtn.unitsize );
    rtn.p = copyto;
    return rtn;
};
// sh
std::list<calstackele> _lists;

private:

bool evaluateonce(MemoryManagerSingle* mem){
    auto iter = _lists.begin();
    if( ! iter->isoperator){
        LOG(ERROR,VECCALSTACK,"first element should be operator");
        return false;
    }
    bool findvaluableiter = false;
    while(!(findvaluableiter || iter == _lists.end())){
        // current iter should be pointing to the operator
        int numarg = iter->opr->argn;
        std::list<calstackele>::iterator iterarg(iter);
        iterarg++;
        int succeednum=0;
        for(int j=0;j<numarg;j++){
            if(iterarg == _lists.end()){
                LOG(ERROR,VECCALSTACK,"number of args are not right, " << "required "<<numarg<<" received"<< j+1 << std::endl);
                iter = iterarg;
                break;
            }
            else if(iterarg->isoperator){
                iter = iterarg; // update to the latest operator
                break;
            }
            else{
               succeednum++; // need 
            }
        }
        if(succeednum==numarg && iter!=iterarg){
            LOG(DEBUG,VECCALSTACK,"find the evaluable operator with n args "<< succeednum <<std::endl );
            findvaluableiter = true;
        }
        // remaining cases, either loop to the end, or we need further evaluation on next operator
    }

    if(findvaluableiter){
        decltype(iter) iterarg(iter);
        int numarg = iter->opr->argn;
        for(int j=0;j<numarg;j++){
            iterarg++;
            iter->opr->regarg(iterarg->oprd);
        }
        varx rtn = iter->opr->evaluate(mem);
        if (!rtn){
            LOG(ERROR,VECCALSTACK,"error happend in evaluation of operator with "<< iter->opr->argn << "args" << std::endl);
            return false;
        }
        for(int j=0;j<numarg+1;j++){
            iter = _lists.erase(iter);
        }
        _lists.insert(iter,rtn);
        return true;   
    }
    else{
        LOG(ERROR,VECCALSTACK,"no available evaluation find"<< std::endl);
        return false;// tell caller, no evaluation
    }
};


};

// overloading functions
calcstack operator+(calstackele ele,calcstack second);
calcstack operator+(calstackele first,calstackele second);

// var or operator will be promoted to  ele ,  make concatnation start from CalcStack






// implementation, 

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


#define HEADEREXP_H(n) varx x##n
#define HEADEREXP_B(n) ,varx x##n
#define HEADEREXP_N(n) REPEAT(n, HEADEREXP_H, HEADEREXP_B)

#define ARGSATX(x) args.at(x - 1)
#define VECARGEXP_H(n) ARGSATX(n)
#define VECARGEXP_B(n) ,ARGSATX(n)
#define VECARGEXP_N(n) REPEAT(n, VECARGEXP_H, VECARGEXP_B)



 // where operator?, these standard operators, could be some standard one, but also extend the interface (or )
 // n < 9
 // first argument is the output
 #define VECOPERATORN(n) \
 class vecopr_##n:public stackvecopr{public:    \
     vecopr_##n():stackvecopr(n){};      \
     virtual varx evaluatenotype(HEADEREXP_N(n),MemoryManagerSingle* mem )=0;\
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

#define EXTENDOPERATOR(n) virtual varx evaluatenotype(HEADEREXP_N(n),MemoryManagerSingle* mem){decltype(this->operator()(varx(),SIMPLEXIEXP_N(n))) y; auto l = rtnsingle?1:returnlen(SIMPLEXIEXP_N(n)); \
 auto unit =  y.unitsize();std::cout << "returned allocated size" << l << " unit of T size" <<  unit << std::endl    ; y.p = static_cast<decltype(y.p)>(mem->allocate(l*unit)); y.l= l; auto z = this->operator()(y,SIMPLEXIEXP_N(n));return z;};


// varx can be casted to/from any pointer with length



};




#endif /* ARRAYCALC_H */
