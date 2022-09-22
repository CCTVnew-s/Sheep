#ifndef __EXTENDARRAYCALCH
#define __EXTENDARRAYCALCH


#include "arraycalc.h"

namespace ARRAY{



// we try to simplify operator, functions and methodoloy
// 1. Need to preallocate the space for result, rtn->length can be reduced , with temparariry memory leakage
// 2. It wanted to supoort chained calculation in CalcStack, it needs to provide information e.g. what's the return type (size) and dimonsion for intermidate results
// 3. If calculation failed, rtn won't be the same as argument rtn, such case, need to handle, especially we can't recollect the space allocated
// 4. whether we support inplace updates ??? seems okay, when you code the operators carefully
// 5. how to easy extend function into vector function,,, This is actually wraping f function with adverb,,parallel, scan, over??
// 6. Usage perspective, a lot of classes converting between, how to simplify

// KEYS of implementation and usage
// 1) implement operator() as above, 
// 2) rtnsingle whether is single value rtn , if not x1 has the same length -- this need to be revisited, x1 should control loop, but 
//      you need some better way determine the return size
// 3) another way of usage  var<T> =  evaluatenotype(x1,x2,,,,xn, mem), mem will take care of the mem, meanwhile you lost memory when sth failed




class Where:public vecopr_1{
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

    // returnlen(HEADEREXP_N(n)){return x1.l;};
    static const bool rtnsingle=false;    // these 2 determines the return size

    EXTENDOPERATOR(1) // normal extension, froced
    
};

#define SIMPLEBIOPR(name,opr)  template <class T1, class T2,  class T3>\
class name:public vecopr_2{ \
    var<T1> operator()(var<T1> rtn, var<T2> x, var<T3> y){ \
        for(int i=0;i<x.l;i++)      \
           rtn[i] = x[i] opr y[i];  \
           return rtn;              \
    }; \
    static const bool rtnsingle=false; \
    EXTENDOPERATOR(2)  \
}; 

// simple ones first
SIMPLEBIOPR(plus,+)
SIMPLEBIOPR(minus,-)
SIMPLEBIOPR(multi,*)
SIMPLEBIOPR(divide,/)


// tgt stay later
template<class T1>
class At:public vecopr_2{

    var<T1> operator()(var<T1> rtn, var<int> loc, var<T1>tgt){
        for(int i=0;i<loc.l;i++)
            rtn[i] = tgt[loc[i]];
        return rtn;
    }
    static const bool rtnsingle=false; 
    EXTENDOPERATOR(2)  
};

// think we better creats as we have more calculation requests





};



#endif