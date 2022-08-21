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


template <typename T>
struct var{
var(T& x):p(&x),l(1){};
var(T* x,int lx): p(x),l(lx){};
T& operator[](int i){return l==1?p[0]:p[i];};
T* p;
int l;
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

class varx{
public:

varx(const varx &v):p(v.p),l(v.l){};
template <typename T>
varx(T& x):p((void*)&x),l(1){};
template <typename T>
varx(T* x,int lx): p((void*)x),l(lx){};

EXPANDVARXCONVERTER(bool)
EXPANDVARXCONVERTER(int)
EXPANDVARXCONVERTER(short)
EXPANDVARXCONVERTER(char)
EXPANDVARXCONVERTER(long)
EXPANDVARXCONVERTER(double)
EXPANDVARXCONVERTER(float)

void* p;
int l;
};




// may not be super useful for simple vector calculation, but used for chained computations
class stackvecopr{
public:
stackvecopr(int n):argn(n), args(std::vector<varx>()){};

virtual bool evaluate(){
    if(args.size()!=argn)
        return false;
    evaluate_imp();
    args.clear(); // clean, just evaluate once
};

virtual bool evaluate_imp() = 0;

virtual bool regarg(varx a){
    args.push_back(a);
};

int argn;
std::vector<varx> args;

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
 class vecopr_##n:public stackvecopr{    \
     vecopr_##n():stackvecopr(n){};      \
     virtual bool evaluatenotype(varx& y,HEADEREXP_N(n)){return false;} ;\
     virtual bool evaluate_imp(){        \
        return evaluatenotype(args.at(0),VECARGEXP_N(n)); };      \
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

#define EXTENDOPERATOR(n) virtual bool evaluatenotype(varx& y,HEADEREXP_N(n)){this->operator()(y,SIMPLEXIEXP_N(n));};

class where:public vecopr_1{
public:
    bool operator()(var<int> rtn, var<bool> cond){
        return false;
    };

    EXTENDOPERATOR(1)
};





#endif /* ARRAYCALC_H */

