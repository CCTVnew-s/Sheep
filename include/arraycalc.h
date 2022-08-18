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

namespace CALC{


template <typename T1, typename T2>
class uniarrayoperator{
    virtual bool operator()(array<T1> &t1, T2& rtn){return false;}; // many to one, rtn one
    virtual bool operator()(array<T1> &t1, array<T2>& rtn){return false;}; // many to one, rtn series   
};

template <typename T1, typename T2, typename T3>
class biarrayopertor{
   
 virtual bool operator()(array<T1> &t1, T2& t2, T3& rtn){return false;}; // many to one, rtn one
 virtual bool operator()(array<T1> &t1, T2& t2, array<T3> &rtn){return false;}; // many to one, rtn series
 virtual bool operator()(array<T1> &t1,array<T2>&, T3& rtn){return false;};// many to many , rtn one
 virtual bool operator()(array<T1> &t1,array<T2>& t2,array<T3>& rtn){return false;}; // return series            
};





};



template <typename T>
struct var{
var(T& x):p(&x),l(1){};
var(T* x,int lx): p(x)),l(x){};
T& operator[](int i){return l==1?p[0]:p[i];};
T* p;
int l;
};


#define DECLAIRETYPEBUILD(type)  var<type> buildvar(type&,int);var<int> buildvar(type*,int);
DECLAIRETYPEBUILD(int)
DECLAIRETYPEBUILD(short)
DECLAIRETYPEBUILD(char)
DECLAIRETYPEBUILD(long)
DECLAIRETYPEBUILD(double)
DECLAIRETYPEBUILD(float)

// max suppot 9 arguments

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







#endif /* ARRAYCALC_H */

